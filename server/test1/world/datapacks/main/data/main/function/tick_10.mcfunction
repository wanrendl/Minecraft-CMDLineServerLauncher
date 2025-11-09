schedule function main:tick_10 10t

#kill items
execute as @e[type=item] unless items entity @s contents *[minecraft:custom_data~{"hielkemaps:stay":true}] run kill