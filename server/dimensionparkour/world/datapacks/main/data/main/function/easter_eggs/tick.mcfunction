execute as @a[gamemode=!spectator,x=27,y=57,z=149,dx=0,dy=1,dz=0,advancements={main:easter_eggs/end_house=false}] at @s run function main:easter_eggs/end_house
execute as @a[gamemode=!spectator,x=17,y=66,z=138,dx=0,dy=1,dz=0,advancements={main:easter_eggs/tree_house=false}] at @s run function main:easter_eggs/tree_house
execute as @a[gamemode=!spectator,x=37,y=69,z=63,dx=3,dy=2,dz=0,advancements={main:easter_eggs/hidden_treasure=false}] at @s run function main:easter_eggs/hidden_treasure
execute as @a[gamemode=!spectator,x=34,y=78,z=35,dx=-2,dy=2,dz=0,advancements={main:easter_eggs/sneak_peek=false}] at @s run function main:easter_eggs/sneak_peek
execute as @a[gamemode=!spectator,x=28,y=76,z=59,distance=..4,advancements={main:easter_eggs/nether_portal=false}] at @s run function main:easter_eggs/nether_portal

execute as @a[gamemode=!spectator,x=-66,y=81,z=203,distance=..5,advancements={main:easter_eggs/larry=false}] at @s run function main:easter_eggs/larry
tp @e[type=magma_cube,tag=ee] -66 81 203

execute as @a[gamemode=!spectator,x=-60,y=100,z=192,distance=..3,advancements={main:easter_eggs/nether_wart_plantation=false}] at @s run function main:easter_eggs/nether_wart_plantation

#ee8 activated by tp/nether_gateway
execute as @a[gamemode=!spectator,x=104,y=75,z=177,dx=0,dy=1,dz=0,advancements={main:easter_eggs/ship=false}] at @s run function main:easter_eggs/ship
execute as @a[gamemode=!spectator,x=130,y=119,z=192,dx=0,dy=1,dz=0,advancements={main:easter_eggs/shelter=false}] at @s run function main:easter_eggs/shelter

scoreboard players add @a[tag=eefound] eedelay 1
execute as @a[scores={eedelay=70..}] at @s run function main:easter_eggs/title