# WeeSleep

(Hacky) Plugin to automatically disconnect weechat when the system is being suspended and reconnect when it wakes up again.

NOTE: THIS (CURRENTLY) ONLY WORKS ON OS X !!!

This plugin tries to solve (it doesn't fully, more of a workaround) the following problem: When weechat is used
in combination with an IRC bouncer (e.g. ZNC) and the system is being suspended, it seems the following happens:

* The TCP connection is not shut down
* ZNC still thinks the client is connected and sends new messages its way
* ZNC marks the messages as delivered
* If the client system is not woken up before the TCP connection is killed due to timeout, those messages are "lost"

So basically, every message received by the bouncer during the 256s TCP timeout period is never seen by the client.
This plugin tries to prevent that from happening by disconnecting weechat when it detects that the system
is going to sleep.

The way the plugin works is as follows. When initializing the plugin, it forks off a child process which
will register with the power subsytem on OS X to receive notifications about suspending. Whenever the child
receives such a notification, it will print something to stdout. The plugin is notified by weechat about
the newly arrived data on the pipe and acts accordingly, by either doing a /disconnect or /reconnect.

To port this plugin to other architectures, only themagic.c has to be changed.

## Usage

```bash
make
make install
```

This will build the binary and .dylib (make sure clang is installed) and copies both to ~/.weechat/plugins.
Now in weechat type "/plugin load weesleep.dylib".
