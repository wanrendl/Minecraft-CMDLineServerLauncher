scoreboard players add @s firework_timer 1
execute if score @s firework_timer matches 5.. run function main:firework/summon
execute if score @s firework_timer matches 100.. run function time:firework/reset