#summon firework using time datapack
function time:firework/summon

execute as @e[type=firework_rocket,limit=1,sort=nearest,tag=!tped] run function main:firework/firework