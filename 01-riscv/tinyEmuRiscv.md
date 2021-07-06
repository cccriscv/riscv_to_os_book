# tinyemu

https://github.com/fernandotcl/TinyEMU

## 安裝失敗

```
mac020:ccc mac020$ brew tap fernandotcl/homebrew-fernandotcl
Updating Homebrew...
==> Auto-updated Homebrew!
Updated Homebrew from d39133102 to e71a0edf6.
Updated 3 taps (dart-lang/dart, homebrew/cask and homebrew/core).
==> New Formulae
bgpq4                                         px
==> Updated Formulae
Updated 104 formulae.
==> New Casks
classroom-mode-for-minecraft                  librewolf
==> Updated Casks
Updated 52 casks.
==> Deleted Casks
adware-removal-tool           foreman                       todos
beoplay-software-update       shoes

==> Tapping fernandotcl/fernandotcl
Cloning into '/usr/local/Homebrew/Library/Taps/fernandotcl/homebrew-fernandotcl'...
remote: Enumerating objects: 130, done.
remote: Counting objects: 100% (25/25), done.
remote: Compressing objects: 100% (15/15), done.
remote: Total 130 (delta 10), reused 21 (delta 6), pack-reused 105
Receiving objects: 100% (130/130), 18.45 KiB | 1.08 MiB/s, done.
Resolving deltas: 100% (65/65), done.
Tapped 6 formulae (21 files, 33.3KB).
mac020:ccc mac020$ brew search tinyemu
==> Formulae
fernandotcl/fernandotcl/tinyemu
mac020:ccc mac020$ brew install tinyemu
Error: fernandotcl/fernandotcl/tinyemu is a head-only formula.
To install it, run:
  brew install --HEAD fernandotcl/fernandotcl/tinyemu
mac020:ccc mac020$ brew install --HEAD fernandotcl/fernandotcl/tinyemu
Warning: You are using macOS 10.12.
We (and Apple) do not provide support for this old version.
You will encounter build failures with some formulae.
Please create pull requests instead of asking for help on Homebrew's GitHub,
Twitter or any other official channels. You are responsible for resolving
any issues you experience while you are running this
old version.

==> Installing tinyemu from fernandotcl/fernandotcl
==> Downloading https://libsdl.org/release/SDL2-2.0.14.tar.gz
######################################################################## 100.0%
==> Cloning https://github.com/fernandotcl/TinyEMU.git
Cloning into '/Users/mac020/Library/Caches/Homebrew/tinyemu--git'...
==> Checking out branch master
Already on 'master'
Your branch is up-to-date with 'origin/master'.
==> Installing dependencies for fernandotcl/fernandotcl/tinyemu: sdl2
==> Installing fernandotcl/fernandotcl/tinyemu dependency: sdl2
==> ./configure --prefix=/usr/local/Cellar/sdl2/2.0.14_1 --without-x --enable-hidapi
==> make install
🍺  /usr/local/Cellar/sdl2/2.0.14_1: 91 files, 5MB, built in 1 minute 52 seconds
==> Installing fernandotcl/fernandotcl/tinyemu --HEAD
==> make CONFIG_MACOS=y
Last 15 lines from /Users/mac020/Library/Logs/Homebrew/tinyemu/01.make:
fs_disk.c:458:33: error: use of undeclared identifier 'UTIME_NOW'
                ts[0].tv_nsec = UTIME_NOW;
                                ^
fs_disk.c:462:29: error: use of undeclared identifier 'UTIME_OMIT'
            ts[0].tv_nsec = UTIME_OMIT;
                            ^
fs_disk.c:470:33: error: use of undeclared identifier 'UTIME_NOW'
                ts[1].tv_nsec = UTIME_NOW;
                                ^
fs_disk.c:474:29: error: use of undeclared identifier 'UTIME_OMIT'
            ts[1].tv_nsec = UTIME_OMIT;
                            ^
4 errors generated.
make: *** [fs_disk.o] Error 1
make: *** Waiting for unfinished jobs....

Do not report this issue to Homebrew/brew or Homebrew/core!


Error: You are using macOS 10.12.
We (and Apple) do not provide support for this old version.
You will encounter build failures with some formulae.
Please create pull requests instead of asking for help on Homebrew's GitHub,
Twitter or any other official channels. You are responsible for resolving
any issues you experience while you are running this
old version.

Please create pull requests instead of asking for help on Homebrew's GitHub,
Twitter or any other official channels.
mac020:ccc mac020$ temu
bash: temu: command not found
```