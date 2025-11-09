#kill dragon 2 ticks later otherwise end crystals bug out
schedule function main:win/kill_dragon 2t

tag @e[type=marker,name=console] add cooldown
tag @a[tag=bossbattle] add hasWon

scoreboard players set @a[tag=bossbattle] end_timer -100
execute as @a[tag=bossbattle,gamemode=!spectator] run function time:finish