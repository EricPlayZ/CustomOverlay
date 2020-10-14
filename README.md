# Custom Overlay 2.0
A BakkesMod plugin which uses a custom overlay in-game to improve FPS.

Since this plugin was already made by JerryTheBee ([his plugin page](https://bakkesplugins.com/plugins/view/148) if you wanna check it out), I've named this 2.0 because, you could say, this is a remastered version of his plugin.

Why did I "remaster" it? Because it had a few issues:
1. The biggest issue was resizing text based on your screen resolution. I tried his plugin and the text was all over the screen, I couldn't see anything. And I actually saw an improvement in FPS, so instead of just going back with using the default UI rendering, I decided to remake the plugin from scratch. I don't think he shared the source for his plugin anywhere either, so, why not remake it from scratch? I love coding, especially in new languages (in this case, C++).
2. It was kinda janky, as he mentioned in his description. It was really meant to be used only for testing purposes and such, and I took this seriously, because... Well, nvm, I don't think you wanna know my laptop specs lol.

What this plugin does better is: everything, absolutely everything will be 1:1 the same on every single resolution. So no matter your resolution, the UI will be sized exactly the same as at every other resolution! The time is also fixed, separating minutes and seconds, and your team's score and the opponents' team's score are both properly positioned on the scoreboard on top. 

Everything else that his plugin does is also present in my plugin (well, at least from what I can tell), such as automatically disabling the default UI rendering and enabling my custom overlay on kickoff's GO!, or vice versa when a goal gets scored.

I'mma be quoting some of his important stuff from the description in here, in case you've missed it:

##### "In order to get any FPS improvement, you must disable all default rendering. This means you cannot see the game menu, chat, stat events, countdowns, the scoreboard, or time remaining in a tournament match before a tiebreaker. This plugin is objectively a disadvantage to use, but I wanted to see if I could improve FPS for people upset about the move to DX11, and I think I have. You could of course use this plugin with default rendering on, just to get a cool colored scoreboard, but will not see any change in your framerate."

##### "I still recommend binding this to a button that you can easily use and doesn't interfere with your gameplay. This is necessary to use the plugin in freeplay, and is also useful so you can pause and quit or forfeit while not in a goal replay. I like to use a random keyboard button, as I play with controller, so I can do it often but it doesn't take a button on a small controller. Press f6 and copy whichever of these commands makes most sense for you with your choice of button (note that any controller is called XboxTypeS)"

```
bind XboxTypeS_LeftThumbStick "toggle cl_rendering_scaleform_disabled 1 0"
bind P "toggle cl_rendering_scaleform_disabled 1 0"
```

##### "Please be aware that the plugin only supports 2 digit scores (0 - 99)[...] Anything above those two restrictions will just look bad."
And the timer at the top only supports 5 characters (which includes the colon), so basically e.g. "99:59", everything above will "stretch" the text out of the center box and will make it look ugly. Yeah man, this plugin isn't perfect either lol, if you got any solutions to that, visit my GitHub repo and make a suggestion.

Now finally, if you want to contribute to the plugin in any sort of way, or if you have any issues or if you just wanna see the source code (which isn't great btw, it's my first C++ project lol), well um... You're on the repository right now, so feel free to do any of that.
