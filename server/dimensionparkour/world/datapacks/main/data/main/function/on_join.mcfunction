attribute @s minecraft:burning_time base set 0.001

tp @s 20 60 146 180 5
gamemode adventure @s
function main:reset_scores

advancement revoke @s from main:easter_eggs/root
scoreboard players set @s eefound 0

clear @s *[minecraft:custom_data~{"hielkemaps:item":true}]
execute as @s[tag=training_mode] run function trainingmode:leave

tag @s add joined