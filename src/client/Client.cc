/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Client.cc
 *
 * Game initialisation and main loop.
 */

#include <client/Client.hh>

#include <client/Network.hh>
#include <client/Input.hh>
#include <client/Context.hh>
#include <client/Sound.hh>
#include <client/Render.hh>
#include <client/Loader.hh>
#include <client/BuildInfo.hh>
#include <client/MenuStage.hh>
#include <client/GameStage.hh>
#include <client/EditStage.hh>
#include <client/ui/UI.hh>

#include <SDL.h>
#include <SDL_ttf.h>
#include <unistd.h>
#ifdef __native_client__
# include <ppapi_simple/ps.h>
# include <ppapi_simple/ps_event.h>
# include <ppapi_simple/ps_interface.h>
# include <sys/mount.h>
#endif

#undef main

namespace oz
{
namespace client
{

#ifdef __native_client__
static const PPB_InputEvent*      ppbInputEvent      = nullptr;
static const PPB_MouseInputEvent* ppbMouseInputEvent = nullptr;
#endif

void Client::printUsage()
{
  Log::printRaw(
    "Usage: openzone [-v] [-l | -i <mission>] [-t <num>] [-L <lang>] [-p <prefix>]\n"
    "  -l            Skip main menu and load the last autosaved game.\n"
    "  -i <mission>  Skip main menu and start mission <mission>.\n"
    "  -e <layout>   Edit world <layout> file. Create a new one if non-existent.\n"
    "  -t <num>      Exit after <num> seconds (can be a floating-point number) and\n"
    "                use 42 as the random seed. Useful for benchmarking.\n"
    "  -L <lang>     Use language <lang>. Should match a subdirectory name in\n"
    "                'lingua/' directory inside game data archives.\n"
    "                Defaults to 'en'.\n"
    "  -p <prefix>   Set global data directory to '<prefix>/share/openzone'.\n"
    "                Defaults to '%s'.\n"
    "  -v            Print verbose log messages to terminal.\n\n", OZ_PREFIX);
}

int Client::main()
{
  SDL_Event event;

  bool isAlive   = true;
  bool isActive  = true;
  // Time spent on the current frame so far.
  uint timeSpent = 0;
  uint timeZero  = Time::uclock();
  // Time at the end of the last frame.
  uint timeLast  = timeZero;

  initFlags |= INIT_MAIN_LOOP;

  Log::println("Main loop {");
  Log::indent();

  // THE MAGNIFICENT MAIN LOOP
  do {
    // read input & events
    input.prepare();

#ifdef __native_client__

    PSEventSetFilter(PSE_INSTANCE_HANDLEINPUT | PSE_INSTANCE_DIDCHANGEVIEW |
                     PSE_MOUSELOCK_MOUSELOCKLOST);

    List<PSEvent*> eventQueue;
    PSEvent*       psEvent;

    while ((psEvent = PSEventTryAcquire()) != nullptr) {
      switch (psEvent->type) {
        case PSE_INSTANCE_HANDLEINPUT: {
          PP_InputEvent_Type type = ppbInputEvent->GetType(psEvent->as_resource);

          switch (type) {
            case PP_INPUTEVENT_TYPE_MOUSEMOVE: {
              PP_Point point = ppbMouseInputEvent->GetMovement(psEvent->as_resource);

              input.mouseX += point.x;
              input.mouseY -= point.y;
              break;
            }
            default: {
              eventQueue.add(psEvent);
              continue;
            }
          }
          break;
        }
        case PSE_INSTANCE_DIDCHANGEFOCUS: {
          Window::setFocus(psEvent->as_bool);
          Window::setGrab(psEvent->as_bool);
          input.reset();
          break;
        }
        case PSE_MOUSELOCK_MOUSELOCKLOST: {
          Window::setFocus(false);
          Window::setGrab(false);
          input.reset();
          break;
        }
        case PSE_INSTANCE_DIDCHANGEVIEW: {
          PP_Rect rect;
          PSInterfaceView()->GetRect(psEvent->as_resource, &rect);

          Window::resize(rect.size.width, rect.size.height, Window::isFullscreen());
          input.reset();
          break;
        }
        default: {
          break;
        }
      }

      PSEventRelease(psEvent);
    }

    // Repost relevant events for SDL to process them.
    for (PSEvent* event : eventQueue) {
      PSEventPostResource(event->type, event->as_resource);
    }

    PSEventSetFilter(PSE_ALL);

#endif

    SDL_PumpEvents();

    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_MOUSEMOTION:
        case SDL_MOUSEWHEEL:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_KEYUP: {
          input.readEvent(&event);
          break;
        }
        case SDL_KEYDOWN: {
          const SDL_Keysym& keysym = event.key.keysym;

          if (keysym.sym == SDLK_F9) {
            if (keysym.mod & KMOD_CTRL) {
              ui::ui.isVisible = !ui::ui.isVisible;
            }
#ifndef __native_client__
            else {
              loader.makeScreenshot();
            }
#endif
          }
          else if (keysym.sym == SDLK_F11) {
            if (keysym.mod & KMOD_CTRL) {
              Window::setGrab(!Window::hasGrab());
            }
#ifdef __native_client__
            else {
              Window::resize(Window::width(), Window::height(), !Window::isFullscreen());
            }
#else
            else if (Window::isFullscreen()) {
              Window::resize(windowWidth, windowHeight, false);
            }
            else {
              Window::resize(screenWidth, screenHeight, true);
            }
            input.reset();
#endif
          }
#ifndef __native_client__
          else if (keysym.sym == SDLK_F12) {
            if (keysym.mod & KMOD_CTRL) {
              isAlive = false;
            }
            else {
              Window::minimise();
              input.reset();
            }
          }
#endif

          input.readEvent(&event);
          break;
        }
        case SDL_WINDOWEVENT: {
          switch (event.window.event) {
            case SDL_WINDOWEVENT_FOCUS_GAINED: {
              Window::setFocus(true);
              Window::setGrab(true);
              input.reset();
              break;
            }
            case SDL_WINDOWEVENT_FOCUS_LOST: {
              Window::setFocus(false);
              Window::setGrab(false);
              input.reset();
              break;
            }
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
              Window::updateSize(event.window.data1, event.window.data2);
              input.reset();
              break;
            }
#ifndef __native_client__
            case SDL_WINDOWEVENT_RESTORED: {
              input.reset();
              sound.resume();

              isActive = true;
              break;
            }
            case SDL_WINDOWEVENT_MINIMIZED: {
              sound.suspend();
              input.reset();

              isActive = false;
              break;
            }
            case SDL_WINDOWEVENT_CLOSE: {
              isAlive = false;
              break;
            }
#endif
          }
          break;
        }
        case SDL_QUIT: {
          isAlive = false;
          break;
        }
        default: {
          break;
        }
      }
    }

#ifdef __native_client__

    input.keys[SDLK_ESCAPE]    = false;
    input.oldKeys[SDLK_ESCAPE] = false;

#endif

    // Waste time when iconified.
    if (!isActive) {
      Time::usleep(Timer::TICK_MICROS);

      timeSpent = Time::uclock() - timeLast;
      timeLast += timeSpent;

      continue;
    }

    input.update();

    timer.tick();

    isAlive &= !isBenchmark || timer.time < benchmarkTime;
    isAlive &= stage->update();

    if (Stage::nextStage != nullptr) {
      stage->unload();

      stage = Stage::nextStage;
      Stage::nextStage = nullptr;

      input.prepare();
      input.update();

      stage->load();

      timeLast = Time::uclock();
      continue;
    }

    timeSpent = Time::uclock() - timeLast;

    // Skip rendering graphics, only play sounds if there's not enough time left.
    if (timeSpent >= Timer::TICK_MICROS && timer.frameMicros < 100 * 1000) {
      stage->present(false);
    }
    else {
      stage->present(true);
      timer.frame();

      // If there's still some time left, sleep.
      timeSpent = Time::uclock() - timeLast;

      if (timeSpent < Timer::TICK_MICROS) {
        stage->wait(Timer::TICK_MICROS - timeSpent);
        timeSpent = Timer::TICK_MICROS;
      }
    }

    if (timeSpent > 100 * 1000) {
      timer.drop(timeSpent - Timer::TICK_MICROS);
      timeLast += timeSpent - Timer::TICK_MICROS;
    }
    timeLast += Timer::TICK_MICROS;
  }
  while (isAlive);

  Log::unindent();
  Log::println("}");

  return EXIT_SUCCESS;
}

int Client::init(int argc, char** argv)
{
#ifdef __native_client__
  Pepper::post("init");
#endif

  initFlags     = 0;
  isBenchmark   = false;
  benchmarkTime = 0.0f;

  File   prefixDir  = OZ_PREFIX;
  String language   = "";
  String mission    = "";
  String layoutFile = "";
  bool   doAutoload = false;

  // Standalone. Executable is ./bin/<platform>/openzone.
  if (prefixDir.isEmpty()) {
    prefixDir = File::executable().directory() / "../..";
  }

  optind = 1;
  int opt;
  while ((opt = getopt(argc, argv, "li:e:t:L:p:vhH?")) >= 0) {
    switch (opt) {
      case 'l': {
        doAutoload = true;
        break;
      }
      case 'i': {
        mission = optarg;
        break;
      }
      case 'e': {
        layoutFile = optarg;
        break;
      }
      case 't': {
        const char* end;
        benchmarkTime = float(String::parseDouble(optarg, &end));

        if (end == optarg) {
          printUsage();
          return EXIT_FAILURE;
        }

        isBenchmark = true;
        break;
      }
      case 'L': {
        language = optarg;
        break;
      }
      case 'p': {
        prefixDir = optarg;
        break;
      }
      case 'v': {
        Log::showVerbose = true;
        break;
      }
      default: {
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

#ifdef __native_client__

  // Wait until web page updates game data and sends the language code.
  for (String message = Pepper::pop(); ; message = Pepper::pop()) {
    if (message.isEmpty()) {
      Time::sleep(10);
    }
    else {
      language = message;
      break;
    }
  }

  umount("");
  mount("", "/", "html5fs", 0, "type=TEMPORARY");
  mount("", "/data", "memfs", 0, "");

#endif

  File::init();
  initFlags |= INIT_PHYSFS;

#ifdef __ANDROID__

  File configDir   = OZ_ANDROID_ROOT "/config";
  File dataDir     = OZ_ANDROID_ROOT "/data";
  File picturesDir = "";
  File musicDir    = "";

#else

  File::CONFIG.mkdir();
  File::DATA.mkdir();

  File configDir   = File::CONFIG / "openzone";
  File dataDir     = File::DATA / "openzone";
  File musicDir    = File::MUSIC.isEmpty() ? File::MUSIC : File::MUSIC / "OpenZone";
  File picturesDir = File::PICTURES.isEmpty() ? File::PICTURES : File::PICTURES / "OpenZone";

#endif

  configDir.mkdir();
  (configDir / "saves").mkdir();
  dataDir.mkdir();

  if (Log::init(configDir / "client.log", true)) {
    Log::println("Log file '%s'", Log::file().c());
  }

  Log::println("OpenZone " OZ_VERSION " started on %s", Time::local().toString().c());

  Log::println("Build details {");
  Log::indent();
  Log::println("Date:            %s", BuildInfo::TIME);
  Log::println("Host:            %s", BuildInfo::HOST);
  Log::println("Host arch:       %s", BuildInfo::HOST_ARCH);
  Log::println("Target arch:     %s", BuildInfo::TARGET_ARCH);
  Log::println("Build type:      %s", BuildInfo::BUILD_TYPE);
  Log::println("Compiler:        %s", BuildInfo::COMPILER);
  Log::println("Compiler flags:  %s", BuildInfo::CXX_FLAGS);
  Log::println("Configuration:   %s", BuildInfo::CONFIG);
  Log::unindent();
  Log::println("}");

  dataDir.mountAt("/");

  if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO) != 0) {
    OZ_ERROR("Failed to initialise SDL: %s", SDL_GetError());
  }

  initFlags |= INIT_SDL;

  if (TTF_Init() < 0) {
    OZ_ERROR("Failed to initialise SDL_ttf");
  }
  initFlags |= INIT_SDL_TTF;

  // Clean up after previous versions. Be evil. Delete screenshots.
  File screenshotDir = configDir + "/screenshots";

  for (const File& file : screenshotDir.list()) {
    file.remove();
  }
  screenshotDir.remove();
  (configDir / "client.rc").remove();

  // Load configuration.
  File configFile = configDir + "/client.json";
  if (config.load(configFile) && String::equals(config["_version"].get(""), OZ_VERSION)) {
    Log::printEnd("Configuration read from '%s'", configFile.c());
    initFlags |= INIT_CONFIG;
  }
  else {
    Log::println("Invalid configuration file version, default settings used.");

    config = Json::OBJECT;
    config.add("_version", OZ_VERSION).get(String::EMPTY);
  }

  config.add("dir.config", configDir).get(String::EMPTY);
  config.add("dir.data", dataDir).get(String::EMPTY);
  config.include("dir.pictures", picturesDir).get(String::EMPTY);
  config.include("dir.music", musicDir).get(String::EMPTY);
  config.include("dir.prefix", prefixDir).get(String::EMPTY);

  windowWidth  = config.include("window.windowWidth",  1280).get(0);
  windowHeight = config.include("window.windowHeight", 720 ).get(0);
  screenWidth  = config.include("window.screenWidth",  0   ).get(0);
  screenHeight = config.include("window.screenHeight", 0   ).get(0);

  windowWidth  = windowWidth  == 0 ? Window::desktopWidth()  : windowWidth;
  windowHeight = windowHeight == 0 ? Window::desktopHeight() : windowHeight;
  screenWidth  = screenWidth  == 0 ? Window::desktopWidth()  : screenWidth;
  screenHeight = screenHeight == 0 ? Window::desktopHeight() : screenHeight;

  bool fullscreen = config.include("window.fullscreen", true).get(false);

  Window::create("OpenZone " OZ_VERSION,
                 fullscreen ? screenWidth  : windowWidth,
                 fullscreen ? screenHeight : windowHeight,
                 fullscreen);
  initFlags |= INIT_WINDOW;

  input.init();
  initFlags |= INIT_INPUT;

  network.init();
  initFlags |= INIT_NETWORK;

#ifdef __native_client__

  // Copy game packages to memfs.
  for (const File& package : File("/cache").list("zip")) {
    package.copyTo("/data/openzone/" + package.name());
  }

#endif

  Log::println("Content search path {");
  Log::indent();

#if !defined(__ANDROID__) && !defined(__native_client__)

  File globalDataDir = config["dir.prefix"].get(String::EMPTY) + "/share/openzone";
  File userMusicDir  = config["dir.music"].get(File::MUSIC);

  if (userMusicDir.mountAt("/userMusic", true)) {
    Log::println("%s [mounted on /userMusic]", userMusicDir.c());
  }

#endif

  if (dataDir.mountAt(nullptr, true)) {
    Log::println("%s", dataDir.c());

    for (const File& file : dataDir.list()) {
      if (file.hasExtension("7z") || file.hasExtension("zip")) {
        if (!file.mountAt(nullptr)) {
          OZ_ERROR("Failed to mount '%s' on / in PhysicsFS: %s", file.c(), PHYSFS_getLastError());
        }
        Log::println("%s", file.c());
      }
    }
  }

#if !defined(__ANDROID__) && !defined(__native_client__)

  if (globalDataDir.mountAt(nullptr, true)) {
    Log::println("%s", globalDataDir.c());

    for (const File& file : File(globalDataDir).list()) {
      if (file.hasExtension("7z") || file.hasExtension("zip")) {
        if (!file.mountAt(nullptr)) {
          OZ_ERROR("Failed to mount '%s' on / in PhysicsFS: %s", file.c(), PHYSFS_getLastError());
        }
        Log::println("%s", file.c());
      }
    }
  }

#endif

  Log::unindent();
  Log::println("}");

  config.include("lingua", "AUTO").get(String::EMPTY);

  if (language.isEmpty()) {
    language = config["lingua"].get("AUTO");
  }
  if (language == "AUTO") {
    language = Lingua::detectLanguage("en");
  }

  Log::print("Setting language '%s' ...", language.c());
  if (lingua.init(language)) {
    Log::printEnd(" OK");

    initFlags |= INIT_LINGUA;
  }
  else {
    Log::printEnd(" Failed");
  }

  config.include("seed", "TIME");

  int seed;

  if (config["seed"].type() == Json::STRING) {
    if (config["seed"].get(String::EMPTY) != "TIME") {
      OZ_ERROR("Configuration variable 'sees' must be either \"TIME\" or an integer");
    }

    seed = int(Time::epoch());
  }
  else {
    seed = config["seed"].get(42);
  }

  if (isBenchmark) {
    seed = 42;
  }

  Math::seed(seed);
  Lua::randomSeed = seed;

  Log::println("Random generator seed set to: %u", seed);

  sound.initLibs();

  initFlags |= INIT_LIBRARY;
  liber.init(config["dir.music"].get(""));

  initFlags |= INIT_CONTEXT;
  context.init();

  initFlags |= INIT_AUDIO;
  sound.init();

  initFlags |= INIT_RENDER;
  render.init();

  initFlags |= INIT_STAGE_INIT;
  menuStage.init();
  gameStage.init();

#ifdef __native_client__
  Pepper::post("none");

  ppbInputEvent      = PSInterfaceInputEvent();
  ppbMouseInputEvent = static_cast<const PPB_MouseInputEvent*>(
                         PSGetInterface(PPB_MOUSE_INPUT_EVENT_INTERFACE));
#endif

  Stage::nextStage = nullptr;

  if (!layoutFile.isEmpty()) {
    editStage.layoutFile = layoutFile;

    stage = &editStage;
  }
  else if (!mission.isEmpty()) {
    gameStage.mission = mission;

    stage = &gameStage;
  }
  else if (doAutoload) {
    gameStage.stateFile = gameStage.autosaveFile;
    stage = &gameStage;
  }
  else {
    stage = &menuStage;
  }

  stage->load();

#ifndef __native_client__
  Window::setGrab(true);
#endif
  input.reset();

  return EXIT_SUCCESS;
}

void Client::shutdown()
{
  if (stage != nullptr) {
    stage->unload();
  }

  if (initFlags & INIT_STAGE_INIT) {
    gameStage.destroy();
    menuStage.destroy();
  }
  if (initFlags & INIT_RENDER) {
    render.destroy();
  }
  if (initFlags & INIT_AUDIO) {
    sound.destroy();
  }
  if (initFlags & INIT_CONTEXT) {
    context.destroy();
  }
  if (initFlags & INIT_LIBRARY) {
    liber.destroy();
  }
  if (initFlags & INIT_LINGUA) {
    lingua.destroy();
  }
  if (initFlags & INIT_NETWORK) {
    network.destroy();
  }
  if (initFlags & INIT_INPUT) {
    input.destroy();
  }
  if (initFlags & INIT_WINDOW) {
    Window::destroy();
  }
  if (initFlags & INIT_MAIN_LOOP) {
    File configFile = config["dir.config"].get(File::CONFIG) + "/client.json";

    if (!(initFlags & INIT_CONFIG)) {
      config.exclude("dir.config");
      config.exclude("dir.data");

      Log::print("Writing configuration to '%s' ...", configFile.c());
      config.save(configFile, CONFIG_FORMAT);
      Log::printEnd(" OK");
    }
  }

  config.clear(initFlags & INIT_CONFIG);

  if (initFlags & INIT_SDL_TTF) {
    TTF_Quit();
  }
  if (initFlags & INIT_SDL) {
    // HACK Crashes on NaCl.
#ifndef __native_client__
    SDL_Quit();
#endif
  }
  if (initFlags & INIT_PHYSFS) {
    File::destroy();
  }

  Log::printProfilerStatistics();
  Profiler::clear();

  Log::printMemorySummary();

  if (initFlags) {
    Log::println("OpenZone " OZ_VERSION " finished on %s", Time::local().toString().c());
  }

#ifdef __native_client__
  Pepper::post("quit");
#endif
}

Client client;

}
}
