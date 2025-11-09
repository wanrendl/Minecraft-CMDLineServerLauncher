execute store result score #random value run random value 10..35
execute store result storage minecraft:main lifetime int 1 run scoreboard players get #random value
data modify entity @s LifeTime set from storage minecraft:main lifetime

spreadplayers 105 184 0 25 under 255 false @s
tag @s add tped