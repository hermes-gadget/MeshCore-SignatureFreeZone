#pragma once
#include <stdint.h>
#include <stddef.h>

// ---------------------------------------------------------------------------
// Signature-Free Zone (SFZ) -- a joke build for the MeshCore repeater firmware.
//
// Detects MeshOS "signed channel" messages (companion app v1.0.26+): the app
// appends an ending to the message text, inside the encrypted payload:
//
//   plaintext: [timestamp 4B][flags 1B]["<Name>: <text>"][0x00]
//              [0x53 0x01][signer key id 8B][Ed25519 signature 64B][zero pad <= 15B]
//
// This scans DECRYPTED group-text plaintext and returns true when the message
// carries such an ending. Detection is structural only -- signatures are not
// (and cannot be) verified here. Marker bytes 0x53 0x01 are 'S' + version 1,
// per the format reverse-engineered on the UK mesh (Oct 2026).
// ---------------------------------------------------------------------------
static inline bool isSignedChannelMessage(const uint8_t* data, size_t len) {
  if (len < 6) return false;
  size_t i = 5;                                  // skip timestamp(4) + flags(1)
  while (i < len && data[i] != 0x00) i++;        // find the end of the text
  if (i >= len) return false;                    // no terminator found
  size_t ext = i + 1;                            // signed ending starts here
  size_t ext_end = ext + 74;                     // 0x53 0x01 + 8B key id + 64B sig
  if (ext_end > len) return false;
  if (data[ext] != 0x53 || data[ext + 1] != 0x01) return false;
  if (len - ext_end > 15) return false;          // at most one block of padding
  for (size_t j = ext_end; j < len; j++) {
    if (data[j] != 0x00) return false;           // tail must be pure zero padding
  }
  return true;
}
