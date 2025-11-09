tp @s 24 27 3 0 10
gamemode adventure @s
function main:reset_scores

clear @s *[minecraft:custom_data~{"hielkemaps:item":true},!minecraft:custom_data~{"tm:item":true}]