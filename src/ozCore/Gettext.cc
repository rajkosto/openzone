/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozCore/Gettext.cc
 */

#include "Gettext.hh"

#include <cstdlib>
#include <cstring>

namespace oz
{

static const uint GETTEXT_MAGIC = 0x950412de;
static const int  MAX_MESSAGES  = 1 << 16;

struct Gettext::Message
{
  int      original;
  int      translation;
  Message* next;
};

const char* Gettext::systemLanguage(const char* fallback)
{
  for (const char* envVar : { "LC_ALL", "LC_MESSAGES", "LANG" }) {
    const char* lang = getenv(envVar);

    if (lang != nullptr && !String::isEmpty(lang)) {
      return lang;
    }
  }
  return fallback;
}

Gettext::~Gettext()
{
  clear();
}

Gettext::Gettext(Gettext&& gt) :
  table(gt.table), messages(gt.messages), strings(gt.strings), nBuckets(gt.nBuckets),
  nMessages(gt.nMessages), stringsSize(gt.stringsSize)
{
  gt.table       = nullptr;
  gt.messages    = nullptr;
  gt.strings     = nullptr;
  gt.nBuckets    = 0;
  gt.nMessages   = 0;
  gt.stringsSize = 0;
}

Gettext& Gettext::operator = (Gettext&& gt)
{
  if (&gt != this) {
    clear();

    table       = gt.table;
    messages    = gt.messages;
    strings     = gt.strings;
    nBuckets    = gt.nBuckets;
    nMessages   = gt.nMessages;
    stringsSize = gt.stringsSize;

    gt.table       = nullptr;
    gt.messages    = nullptr;
    gt.strings     = nullptr;
    gt.nBuckets    = 0;
    gt.nMessages   = 0;
    gt.stringsSize = 0;
  }
  return *this;
}

bool Gettext::contains(const char* message) const
{
  if (nMessages == 0 || String::isEmpty(message)) {
    return false;
  }

  uint index = uint(Hash<const char*>()(message)) % uint(nBuckets);

  for (const Message* m = table[index]; m != nullptr; m = m->next) {
    if (String::equals(strings + m->original, message)) {
      return true;
    }
  }
  return false;
}

const char* Gettext::get(const char* message) const
{
  if (nMessages == 0 || String::isEmpty(message)) {
    return message;
  }

  uint index = uint(Hash<const char*>()(message)) % uint(nBuckets);

  for (const Message* m = table[index]; m != nullptr; m = m->next) {
    if (String::equals(strings + m->original, message)) {
      return strings + m->translation;
    }
  }
  return message;
}

List<const char*> Gettext::catalogueDescriptions() const
{
  List<const char*> descriptions;

  for (int i = 0; i < nMessages; ++i) {
    if (String::isEmpty(strings + messages[i].original)) {
      descriptions.add(strings + messages[i].translation);
    }
  }
  return descriptions;
}

// .mo file layout can be found at http://www.gnu.org/software/gettext/manual/gettext.html#MO-Files.
bool Gettext::import(const File& file)
{
  Stream is = file.read();
  if (is.available() == 0) {
    return false;
  }

  // Header.
  uint magic = is.readUInt();
  if (magic != GETTEXT_MAGIC) {
    if (Endian::bswap(magic) == GETTEXT_MAGIC) {
      is.order = Endian::Order(!is.order);
    }
    else {
      return false;
    }
  }

  is.readInt();
  int nNewMessages = is.readInt();
  if (nNewMessages <= 0 || nNewMessages > MAX_MESSAGES) {
    return nNewMessages == 0;
  }

  int originalsOffset    = is.readInt();
  int translationsOffset = is.readInt();
  int hashtableOffset    = is.readInt();
  int hashtableSize      = is.readInt();
  int stringsOffset      = hashtableOffset + hashtableSize;
  int newStringsSize     = is.capacity() - stringsOffset;

  // Expand messages and strings arrays.
  messages = Arrays::reallocate<Message>(messages, nMessages, nMessages + nNewMessages);
  strings  = Arrays::reallocate<char>(strings, stringsSize, stringsSize + newStringsSize);

  // Add new message entries.
  for (int i = 0; i < nNewMessages; ++i) {
    is.seek(originalsOffset + i * 8 + 4);
    messages[nMessages + i].original = stringsSize + (is.readInt() - stringsOffset);

    is.seek(translationsOffset + i * 8 + 4);
    messages[nMessages + i].translation = stringsSize + (is.readInt() - stringsOffset);
  }

  // Add new strings.
  is.seek(stringsOffset);
  memcpy(strings + stringsSize, is.readSkip(newStringsSize), newStringsSize);

  nMessages   += nNewMessages;
  stringsSize += newStringsSize;

  // Rebuild hashtable.
  nBuckets = (4 * nMessages) / 3;

  delete[] table;
  table = new Message*[nBuckets] {};

  for (int i = 0; i < nMessages; ++i) {
    const char* original = strings + messages[i].original;

    if (String::isEmpty(original)) {
      continue;
    }

    uint index = uint(Hash<const char*>()(original)) % uint(nBuckets);

    messages[i].next = table[index];
    table[index] = &messages[i];
  }

  return true;
}

void Gettext::clear()
{
  delete[] table;
  delete[] messages;
  delete[] strings;

  table       = nullptr;
  messages    = nullptr;
  strings     = nullptr;
  nBuckets    = 0;
  nMessages   = 0;
  stringsSize = 0;
}

}
