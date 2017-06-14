# Kaleidoscope-Focus

![status][st:experimental] [![Build Status][travis:image]][travis:status]

 [travis:image]: https://travis-ci.org/keyboardio/Kaleidoscope-Focus.svg?branch=master
 [travis:status]: https://travis-ci.org/keyboardio/Kaleidoscope-Focus

 [st:stable]: https://img.shields.io/badge/stable-✔-black.svg?style=flat&colorA=44cc11&colorB=494e52
 [st:broken]: https://img.shields.io/badge/broken-X-black.svg?style=flat&colorA=e05d44&colorB=494e52
 [st:experimental]: https://img.shields.io/badge/experimental----black.svg?style=flat&colorA=dfb317&colorB=494e52

Bidirectional communication for Kaleidoscope. With this plugin, one can expose a
set of commands via the Serial port, and allow the host to talk with the
keyboard - and vice versa. This plugin implements only the basic building
blocks, a framework other plugins can opt-in to.

## Using the plugin

This plugin is **not** meant to be used by the end-user (apart from setting it
up to use plugin-provided hooks), but by plugin authors instead. As an end user,
please see the documentation of the plugins you use, for instructions on how to
hook them up with `Focus`!

Nevertheless, the basic commands we implement with this plugin alone, are usable
like this:

```c++
#include <Kaleidoscope.h>
#include <Kaleidoscope-Focus.h>

void setup () {
  Serial.begin (9600);

  USE_PLUGINS (&Focus);
  
  Kaleidoscope.setup ();

  Focus.addHook (FOCUS_HOOK_HELP);
  Focus.addHook (FOCUS_HOOK_VERSION);
}
```

## Plugin methods

The plugin provides the `Focus` object, which has the following method:

### `.addHook(FOCUS_HOOK (function, documentation))`

> Adds a new hook to `Focus`. Hooks are called in order of registration, and
> they get the parsed command name as argument. If they handle the command, they
> shall return `true`, otherwise `false`. Once a command has been handled, it
> will not be given to other hooks.
>
> The hook function is responsible for reading the rest of the command, in
> whatever way it sees fit.
>
> The `documentation` argument is a string, used by the `help` command, and can
> be left empty, if no documentation is desired for the `function`.

## Focus commands

The plugin ships with two (optional) hooks: `FOCUS_HOOK_VERSION`, and
`FOCUS_HOOK_HELP`, implementing the following two commands, respectively:

### `version`

> Return the version of the firmware, the keyboard vendor & product, and the
> compile date.

### `help`

> Return the list of commands the keyboard supports.

## Wire protocol

`Focus` uses a simple, textual, request-response-based wire protocol. 

Each request has to be on one line, anything before the first space is the
command part (if there is no space, just a newline, then the whole line will be
considered a command), everything after are arguments. The plugin itself only
parses until the end of the command part, argument parsing is left to the
various hooks. If there is anything left on the line after hooks are done
processing, it will be ignored.

Responses can be multi-line, but most aren't. Their content is also up to the
hooks, `Focus` does not enforce anything, except a trailing dot and a newline.
Responses should end with a dot on its own line.

Apart from these, there are no restrictions on what can go over the wire, but to
make the experience consistent, find a few guidelines below:

* Commands should be namespaced, so that the plugin name, or functionality comes
  first, then the sub-command or property. Such as `led.theme`, or `led.setAll`.
* One should not use setters and getters, but a single property command instead.
  One, which when called without arguments, will act as a getter, and as a
  setter otherwise.
* Namespaces should be lowercase, while the commands within them camel-case.
* Do as little work in the hooks as possible. While the protocol is human
  readable, the expectation is that tools will be used to interact with the
  keyboard.
* As such, keep formatting to the bare minimum. No fancy table-like responses.
* In general, the output of a getter should be copy-pasteable to a setter.

These are merely guidelines, and there can be - and are - exceptions. Use your
discretion when writing Focus hooks.

### Example

In the examples below, `<` denotes what the host sends to the keyboard, `>` what
the keyboard responds.

```
< version
> Kaleidoscope/locally-built Keyboardio/Model 01 | Jun 14 2017 11:22:19
> .
```

```
< help
> help
> version
> .
```

```
< palette
> 0 0 0 128 128 128 255 255 255
> .
< palette 0 0 0 128 128 128 255 255 255
> .
```

## Further reading

Starting from the [example][plugin:example] is the recommended way of getting
started with the plugin.

  [plugin:example]: https://github.com/keyboardio/Kaleidoscope-Focus/blob/master/examples/Focus/Focus.ino
