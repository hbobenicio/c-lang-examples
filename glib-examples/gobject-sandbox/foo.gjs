#!/usr/bin/env gjs
imports.gi.versions.Gtk = "3.0";
const { Gtk } = imports.gi;

Gtk.init(null);

/* create a widget to demonstrate */

const window = new Gtk.Window({});
window.set_title('My Window');
window.show_all();

Gtk.main();

