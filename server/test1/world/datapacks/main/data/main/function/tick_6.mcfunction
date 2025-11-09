schedule function main:tick_6 6t

#Custom firework implementation because dimension parkour needs more space 
execute as @e[type=marker,name=fireworks,limit=1,tag=fire] at @s run function main:firework/timer