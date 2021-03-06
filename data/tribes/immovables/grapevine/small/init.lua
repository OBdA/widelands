push_textdomain("tribes")

dirname = path.dirname(__file__)

descriptions:new_immovable_type {
   name = "grapevine_small",
   -- TRANSLATORS: This is an immovable name used in lists of immovables
   descname = pgettext("immovable", "Grapevine (small)"),
   icon = dirname .. "menu.png",
   size = "medium",
   programs = {
      main = {
         "animate=idle duration:28s",
         "transform=grapevine_medium",
      }
   },

   animations = {
      idle = {
         pictures = path.list_files(dirname .. "idle_??.png"),
         hotspot = { 15, 18 },
      },
   }
}

pop_textdomain()
