# Signature-Free Zone (SFZ)

A stock **MeshCore repeater firmware** with one opinion: **it does not relay MeshOS "signed channel messages."**

Built for laughs in October 2026, after the MeshOS signed-messages feature landed on the UK mesh.
It is a joke firmware — a counter-toy. Not affiliated with, endorsed by, or supported by the
MeshCore project or the MeshOS project.

## What it does

- After a group message is decrypted, the repeater checks for the MeshOS signed-message ending:
  `0x00 0x53 0x01 <signer key id 8B> <Ed25519 signature 64B>` (zero-padded to the AES block boundary).
- If present: the packet is marked **do-not-retransmit** — this node refuses to carry it onwards — and a line is printed on serial:
  `[SFZ] signed message blocked - this repeater is a signature-free zone. (blocked: N)`
- The Public channel is sniffed with the well-known key (hash byte `0x11`), so signed messages on
  Public are always caught — even when the node has no channel keys configured. Any *other* channel
  is checked whenever this node can decrypt it.

## Build

    pio run -e Heltec_v3_repeater_sfz

Every repeater board has an `_sfz` twin of its stock `<Board>_repeater` env (91
targets). Build them all with `FIRMWARE_VERSION=<ver> sh build.sh build-sfz-firmwares`,
or run the **Build SFZ Firmwares** GitHub Actions workflow to build the whole set
and attach it to a draft release.

Merged image output lands in `.pio/build/<env>/` (via `merge-bin.py`).

## Flash

    pio run -e Heltec_v3_repeater_sfz -t upload

or esptool with the merged `.bin` (write at offset `0x0`). **Back up your current flash first.**

## Test

    pio test -e native -f test_signature_free_zone

## Caveats (it's a joke, but honest)

- Only blocks messages this node can **decrypt** (Public: always; other channels: only if configured).
- Detection is **structural** — it does not verify signatures (it couldn't).
- Only affects **this node's relaying** — the rest of the mesh happily carries signed messages.
- No effect on direct messages, adverts, or anything unsigned.

Full notes: [SIGNATURE_FREE_ZONE.md](SIGNATURE_FREE_ZONE.md)

## Upstream

Based on stock MeshCore (tag `repeater-v1.17.1` — <https://github.com/meshcore-dev/MeshCore>),
MIT licensed; see [license.txt](license.txt). For MeshCore itself, get it from upstream.
