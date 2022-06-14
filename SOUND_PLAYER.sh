#!/usr/bin/env bash

if hash mpv 2>/dev/null; then
    echo "mpv "
elif hash vlc 2>/dev/null; then
    echo "cvlc "
elif hash xdg-open 2>/dev/null; then
    echo "xdg-open "
elif hash rhythmbox 2>/dev/null; then
    echo "rhythmbox "
else
    exit 1
fi
