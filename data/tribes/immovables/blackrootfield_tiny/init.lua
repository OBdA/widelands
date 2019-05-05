dirname = path.dirname(__file__)

tribes:new_immovable_type {
   msgctxt = "immovable",
   name = "blackrootfield_tiny",
   -- TRANSLATORS: This is an immovable name used in lists of immovables
   descname = pgettext("immovable", "Blackroot Field (tiny)"),
   helptext_script = dirname .. "helptexts.lua",
   size = "small",
   attributes = { "field", "seed_blackroot" },
   programs = {
      program = {
         "animate=idle 30000",
         "transform=blackrootfield_small",
      }
   },

   animations = {
      idle = {
         pictures = path.list_files(dirname .. "idle_??.png"),
         hotspot = { 26, 16 },
      },
   }
}
