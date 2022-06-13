# Alarm
A command line alarm. Can set multiple simultanous alarms, that each sound different (or not!)

## Set up :

Use the `make` command in the directory to compile the code into an executable.

You can now run **alarm** by simply using the command `alarm`

But that is simply not enough!

## How do I run this ?

**alarm** calls a set of function for any given option. Some of these functions will need arguments, and you are therefore expected to pass them after calling said function.

These are the functions you may use:

  - `--help` : will give you the info you need to use this program !
  - `--play` : sets the music to be played by all subsequent alarms. If play is not set, the alarm will only consist of a few pings by your system.
  - `--at` : sets a new alarm to ring at a specific time
  - `--in` : sets a new alarm to ring after a given duration

All times use a base of 24h (for our American friends, 14h is 2PM and so on).

## Example:

```bash
./alarm --play ~/Music/tune.mp3 --at 02h00
```
