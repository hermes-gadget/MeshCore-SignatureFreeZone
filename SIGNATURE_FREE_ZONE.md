# Signature-Free Zone (SFZ) — a joke firmware

A stock **MeshCore repeater firmware** (v1.17.1, Heltec V3) with one opinion:
**it does not relay signed messages.** Built for laughs, October 2026, after the
MeshOS "signed channel messages" feature landed on the UK mesh.

## What it does

- After a group message is decrypted, the repeater checks for the MeshOS
  signed-message ending: `0x00 0x53 0x01 <signer key id 8B> <Ed25519 signature 64B>`
  right after the message text (zero-padded to the AES block boundary).
- If present: the packet is marked **do-not-retransmit** — this node will not
  flood it onwards — and a line is printed on serial:
  `[SFZ] signed message blocked — this repeater is a signature-free zone. (blocked: N)`
- The public channel is sniffed with the well-known key (hash byte `0x11`) even
  if the node has no channel keys configured, so signed messages on Public are
  always caught. Any *other* channel is also checked whenever this node can
  decrypt it.

## Build

    pio run -e Heltec_v3_repeater_sfz

Output in `.pio/build/Heltec_v3_repeater_sfz/` (merged image via `merge-bin.py`).

## Flash

    pio run -e Heltec_v3_repeater_sfz -t upload

or esptool with the merged `.bin`.

## Test

    pio test -e native -f test_signature_free_zone

(Detector unit tests, including real captured signed messages from the UK
public channel; a host-only version of the same vectors was also run with g++.)

## Hardware verification

Flashed to a Heltec V3 bench unit (merged bin @ 0x0): clean boot (ROM log →
`Repeater ID` → SFZ banner), then a 370-second quiet soak — serial completely
silent, `stats-core` at the end returned `{"battery_mv":4034,"uptime_secs":372,
"errors":0,"queue_len":0}` (i.e. zero reboots across the whole window), and
`ver` → `v1.17.1-SFZ`. Note: the serial CLI takes CR-terminated commands and
echoes input; opening the CP2102 port glitches one benign reset (normal for
these boards).

## Scope / caveats (it's a joke, but honest)

- Only blocks messages this node can **decrypt** (Public: always; other
  channels: only if configured).
- Detection is **structural** — it does not verify signatures (it couldn't).
- Only affects **this node's relaying** — the rest of the mesh happily carries
  signed messages.
- No effect on direct messages, adverts, or anything unsigned.

## Diff vs stock

1. `src/helpers/SignatureFreeZone.h` — the detector (header-only).
2. `src/Mesh.cpp` — `#ifdef SIGNATURE_FREE_ZONE` hook at the GRP_TXT decrypt
   site: sniff public channel + mark signed packets do-not-retransmit.
3. `examples/simple_repeater/main.cpp` — boot banner line.
4. `variants/heltec_v3/platformio.ini` — `[env:Heltec_v3_repeater_sfz]`
   (extends the stock repeater env; adds `-D SIGNATURE_FREE_ZONE=1` and a
   `-D FIRMWARE_VERSION` tag).
5. `test/test_signature_free_zone/` — detector tests.
