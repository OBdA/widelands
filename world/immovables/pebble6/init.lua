dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "pebble6",
   descname = _ "Pebble",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "/idle.png" },
         player_color_masks = {},
         hotspot = { 8, 13 }
      },
   }
}
