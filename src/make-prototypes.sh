#!/bin/sh
grep -h "^extern.*;" *.c|sort -u > prototypes.h
