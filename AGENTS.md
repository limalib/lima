# AGENTS.md — LIMA

This tree is a **mudlib** (the game): LPC rooms, objects, commands, login.
The **driver** is a separate program ([FluffOS](https://github.com/fluffos/fluffos)), vendored
as the git submodule [adm/dist/fluffos](adm/dist/fluffos). Do not clone another driver into
this directory. Do not invent efuns.

Onboarding contract: https://www.fluffos.info/llm
Efuns: https://www.fluffos.info/efun/
Applies: https://www.fluffos.info/apply/
LPC: https://www.fluffos.info/lpc/
Config keys: https://www.fluffos.info/driver/config

## Identity

- **Name:** LIMA. `adm/dist/config.mud` is **not committed** (gitignored); it is a local,
  per-checkout file generated from the tracked template [adm/dist/config.mud.dist](adm/dist/config.mud.dist).
  Its `name` ships as the sentinel `My LIMA MUD` — [lib/secure/check_config.c](lib/secure/check_config.c)
  nags at boot until an operator changes it, so never treat a particular `name` value
  (or a particular `mudlib directory`/port) as canonical for this project.
- **Clone:** https://github.com/limalib/lima (`git clone --recurse-submodules`)
- **Language / encoding of game text:** UTF-8
- **Last known driver:** FluffOS master, vendored at [adm/dist/fluffos](adm/dist/fluffos) (prebuilt)
- **Config file:** `adm/dist/config.mud` (local only; see [adm/dist/config.mud.dist](adm/dist/config.mud.dist) for the template)
- **Master object:** `/secure/master` (source: [lib/secure/master.c](lib/secure/master.c))
- **Include directories:** `/include` (relative to `mudlib directory`, see config)
- **Simul-efun file:** `/secure/simul_efun` (source: [lib/secure/simul_efun.c](lib/secure/simul_efun.c))
- **Ports:** telnet 7878 by default (`external_port_1` in config.mud.dist; a local checkout may have changed it)

A second, separate config exists for editor/LPC-language tooling (not the runtime
boot config): [lib/lpc-config.json](lib/lpc-config.json). Keep both in sync if master/simul_efun/
include paths ever change.

LPC source in this tree uses the `.c` extension exclusively (no `.lpc` files).

## How to boot

Driver binaries are prebuilt at [adm/dist/bin/driver](adm/dist/bin/driver) and [bin/driver](bin/driver).
Rebuild only if missing or stale: `cd adm/dist && ./rebuild` (add `-d`/`--database`
only if DB support is actually needed — not required by core LIMA). `./rebuild` also
provisions `config.mud` from `config.mud.dist` on first run if it doesn't exist yet.

```
cd adm/dist
./bin/driver config.mud
```

`config.mud.dist` ships `mudlib directory : ../../lib` and `log directory : ../../lib/log`,
relative to `adm/dist` (where the driver is started from) — portable across any clone.
A local `config.mud` may override these with absolute paths; don't assume either form.
A healthy boot ends with:

```
Accepting telnet connections on 0.0.0.0:7878.
Initializations complete.
```

`adm/dist/run` wraps the same binary/config with auto-restart-on-reboot-status and
`bg`/`stop` subcommands; prefer the plain binary invocation above for one-off
verification so you aren't left with a long-lived auto-restarting process.

Connect: `telnet 127.0.0.1 <port>` using the port from the local `config.mud` (7878 by default).

Do not reuse `adm/dist/fluffos/testsuite/etc/config.test` on this tree — that harness
is for driver (C++) development only, not this mudlib.

Known cosmetic quirk: boot log can print `Unable to open log file: "<path with leading / dropped>"`
even though `log directory` in config.mud is correct and the directory exists; harmless,
driver falls back to stderr (`noninteractive stderr write` is on).

## Admin / first login

`AUTO_WIZ` is defined in [lib/include/config.h](lib/include/config.h) — the first character
a connecting user creates automatically becomes a guest wizard (no directory of
their own). Use the in-game `admtool` command ([lib/cmds/wiz/admtool.c](lib/cmds/wiz/admtool.c)) to
promote a guest wizard to a full wizard with a domain directory. New wizards start
at `/domains/std/room/Wizroom`.

## Layout

- `lib/secure/` — master object, simul_efun, login/security-critical code
  (`/secure/daemons/` holds daemons needing elevated privilege)
- `lib/daemons/` — shared system state objects, named `*_d.c` (account_d, guild_d,
  quest_d, skill_d, soul_d, channel_d, ...)
- `lib/cmds/` — commands, split into `player/`, `wiz/`, `guild/`, `race/`, `verbs/`;
  non-security commands only (see [lib/cmds/README](lib/cmds/README)); use
  `lib/cmds/player/*` as a template for new commands
- `lib/std/` — inheritable base classes, never cloned directly (`object.c`,
  `living.c`, `base_room.c`, `container.c`, `door.c`, ...)
- `lib/std/modules/` — fine-grained inheritable mixins, named `m_*.c`
  (`m_actions`, `m_wield`, `m_crafting`, `m_npcscript`, `m_test_suite`, ...)
- `lib/std/tests/` — LPC unit tests built on `m_test_suite.c` (see Testing below)
- `lib/domains/` — `std/` (reference/example domain: rooms, monsters, weapons,
  shops, crafting, spells), `common/` (shared cross-domain objects), `other/`
  (actual user domains; copy structure from `common/`, assign wizards via `admtool`)
- `lib/include/` — headers; `config.h` holds compile-time feature flags,
  `global.h` is auto-included in every file (see `global include file` in config)
- `lib/contrib/` — community-contributed code not part of the core lib;
  `lib/contrib/incoming/` is for contributions under review

## Testing

In-game: `unittest <name>` or call `test_d->test_all(1)` for debug output
(runner: [lib/daemons/test_d.c](lib/daemons/test_d.c)). Existing suites live in
`lib/std/tests/` (`items.c`, `limbs.c`, `liquids.c`, `race.c`, `salvage.c`). New
tests inherit [lib/std/modules/m_test_suite.c](lib/std/modules/m_test_suite.c) and override
`init_tests()`, registering cases with `add_test_true()` / `add_test_fail()`.

There is no CI; `unittest`/`test_all` is the only automated check. Run it before
and after a change that touches shared code (daemons, std/, modules/) to catch
regressions the compiler alone won't.

## Hard rules for assistants

1. **Stop and ask** before rewriting combat, currency, or login. Match the
   surrounding style (indent, naming).
2. **Do not invent efuns.** If it is not on https://www.fluffos.info/efun/ or in
   the driver's `src/packages/*/*.spec`, it does not exist. Prefer existing
   simul_efuns ([lib/secure/simul_efun.c](lib/secure/simul_efun.c)) and lib helpers.
3. **Applies** (`create`, `init`, `reset`, `logon`, `connect`, `valid_*`) are driver
   callbacks — signatures at https://www.fluffos.info/apply/. Do not make applies
   `async`.
4. **Do not** boot `adm/dist/fluffos/testsuite/` unless the user is changing
   FluffOS itself (that's the driver repo's own `AGENTS.md`/`CLAUDE.md` territory,
   not this file's).
5. Prefer a **minimal LPC repro** in this mudlib over editing driver C++.
6. Write UTF-8 source; this tree is not GBK.
7. After adding/changing LPC, compile by booting or by reloading the object
   in-game with `update <file>` ([lib/trans/cmds/update.c](lib/trans/cmds/update.c)). A file that
   never loads is not done.
