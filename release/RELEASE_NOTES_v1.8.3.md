# BambuSphere v1.8.3 (beta)

Bambu Cloud fix release on top of v1.8.2. Published as a beta while it gets
real-world testing.

## Release Scope

- **Base**: v1.8.2 beta (camera removal, device IP, Default Screen,
  battery-% fix all included).
- **Toolchain**: ESP-IDF v6.0.2, LVGL v9.5.0.
- **Hardware variants**: AMOLED 1.75, mainline (`all`) and `h2x2d`
  (localnative) printer-target build.

## Fixed: Bambu Cloud login

Upstream PrintSphere's cloud login worked on devices where ours didn't.
Both use identical cloud MQTT; the differences were all in the HTTP login
layer, which is now ported from upstream. The multi-printer cloud session
(instant printer switching) is unchanged.

- **Honest client identity.** Cloud requests now identify as `PrintSphere`
  instead of impersonating OrcaSlicer / `bambu_network_agent`.
- **2FA login with CSRF.** The 2FA step first fetches a CSRF token from
  `/api/csrf` and sends it back, like Bambu's own web sign-in.
- **No more password hammering.** A rejected password is tried once per
  user action (Connect, config change, expired token) instead of every
  15 seconds forever, which could get the account rate-limited or blocked.
- **Backs off from blocks and rate limits.** Cloudflare blocks and rate
  limits are recognised and waited out (`Retry-After`, at least 5 minutes);
  temporary network or server errors retry after about 30–45 seconds.
- **Waits for network time** before any cloud TLS connection.
- **Clearer status in Web Config**, e.g. "blocked by Cloudflare; wait and
  reconnect (HTTP 403)" or "rate limited" instead of a generic failure.

## Known Notes

- If the old firmware's retries got the account rate-limited, Web Config
  may show "rate limited" or "blocked by Cloudflare" at first. Wait for it
  to clear, then press Connect once.
- The local MQTT path can still hang on "waiting for subscribe ack" when
  the printer never acknowledges the subscription; not addressed in this
  release.
- Not yet flashed to a physical device by the maintainer for this build.
