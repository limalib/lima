LIMA Mudlib that work with latest FluffOS driver.

# Get involved
- Visit us at limamudlib.dev 7878 !
- #LIMA Channel on LPC Discord - https://discord.gg/wzUbBgs3AQ

# Documentation
Please see https://docs.limamudlib.dev/

# How to test and contribute
This repo follows [git-flow](https://skoch.github.io/Git-Workflow/).

- `main` — production-grade releases only (stable, tagged).
- `develop` — integration branch for ongoing work; base your contributions here.

```
#Production/stable
git clone --recurse-submodules https://github.com/limalib/lima

#Development/contributing
git clone --recurse-submodules https://github.com/limalib/lima -b develop

cd lima/adm/dist
./rebuild
./run
```

To contribute, branch a `feature/<name>` off `develop` (`git flow feature start <name>`)
and open a pull request against `develop`, not `main`.

# License
GPL, see original License in library as well.
