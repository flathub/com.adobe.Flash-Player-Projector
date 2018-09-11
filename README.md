# Stand-alone Adobe Flash player

The upstream versions of the Flash Player are on [this page](https://www.adobe.com/support/flashplayer/debug_downloads.html).

You can use release-monitoring.org to monitor this page the above page for new versions of the player [via fedmsg](https://release-monitoring.org/project/17854/).

## About network access

Note that networking is disabled on security grounds. There are many security issues being discovered for the Adobe Flash web browser plugin on a regular basis, and we wouldn't want the stand-alone player to have any way to communicate to third-parties. You can work-around this yourself by launching the player with:
```
flatpak run --share=network com.adobe.Flash-Player-Projector
```
