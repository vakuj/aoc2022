# Task day 19

## Summary

From https://adventofcode.com/2022/day/19

---

### Part 1

In order to harvest the clay, you'll need special-purpose clay-collecting robots. To make any type of robot, you'll need ore, which is also plentiful but in the opposite direction from the clay.

Collecting ore requires ore-collecting robots with big drills. Fortunately, `you have exactly one ore-collecting robot` in your pack that you can use to kickstart the whole operation.

Each robot can `+1 of its resource type per minute`. It also takes `one minute for the robot factory` (also conveniently from your pack) to construct any type of robot, although it `consumes the necessary resources available when construction begins`.

The `robot factory has many blueprints` (your puzzle input) you can choose from, but `once you've configured it with a blueprint, you can't change it`. You'll need to work out `which blueprint is best`.

The elephants are starting to look hungry, so you shouldn't take too long; you need to figure out which blueprint would maximize the number of opened geodes after 24 minutes by figuring out which robots to build and when to build them.

Determine the quality level of each blueprint using the largest number of geodes it could produce in 24 minutes. What do you get if you add up the quality level of all of the blueprints in your list?

#### Example

For example:
```
Blueprint 1:
  Each ore robot costs 4 ore.
  Each clay robot costs 2 ore.
  Each obsidian robot costs 3 ore and 14 clay.
  Each geode robot costs 2 ore and 7 obsidian.

Blueprint 2:
  Each ore robot costs 2 ore.
  Each clay robot costs 3 ore.
  Each obsidian robot costs 3 ore and 8 clay.
  Each geode robot costs 3 ore and 12 obsidian.
```
or  = 4*ore
cr  = 2*ore
obr = 3*ore + 14*cl
gr  = 2*ore + 7*obs

bank(0) = [0,0,0,0]
robo(0) = [1,0,0,0]

ore = 4*ore
cl  = 2*ore
obs = 3*ore + 14*cl = 33*ore
ge  = 2*ore + 7*(5*ore + 14*cl) = 37*ore + 98*cl = 233*ore


(Blueprints have been line-wrapped here for legibility. The robot factory's actual assortment of blueprints are provided one blueprint per line.)

Using blueprint 1 in the example above, the largest number of geodes you could open in 24 minutes is 9. One way to achieve that is:

plan: clay, clay, clay, obsidian, clay, obsidian, geode, goede

1. Not enough funds to spend. `1 ore robot +1 ore -> 1 ore`.
1. Not enough funds to spend. `1 ore robot +1 ore -> 2 ore`.
1. Spend 2 ore to start building a clay robot. `1 ore robot +1 ore -> 1 ore`. The new clay robot ready -> 1 total.
1. `1 ore robot +1 ore -> 2 ore`. `1 clay robot +1 clay -> 1 clay`.
1. Spend 2 ore to start building a clay robot. `1 ore robot +1 ore -> 1 ore`. `1 clay robot +1 clay -> 2 clay`. The new clay robot ready -> 2 total.
1. `1 ore robot +1 ore -> 2 ore`. `2 clay robots +2 clay -> 4 clay`.
1. Spend 2 ore to start building a clay robot. `1 ore robot +1 ore -> 1 ore`. `2 clay robots +2 clay -> 6 clay`. The new clay robot ready -> 3 total.
1. `1 ore robot +1 ore -> 2 ore`. `3 clay robots +3 clay -> 9 clay`.
1. `1 ore robot +1 ore -> 3 ore`. `3 clay robots +3 clay -> 12 clay`.
1. `1 ore robot +1 ore -> 4 ore`. `3 clay robots +3 clay -> 15 clay`.
1. Spend 3 ore and 14 clay to start building an obsidian robot. `1 ore robot +1 ore -> 2 ore`. `3 clay robots +3 clay -> 4 clay`. The new obsidian robot ready -> 1 total.
1. Spend 2 ore to start building a clay robot. `1 ore robot +1 ore -> 1 ore`. `3 clay robots +3 clay -> 7 clay`. `1 obsidian robot +1 obsidian -> 1 obsidian`. The new clay robot ready -> 4 total.
1. `1 ore robot +1 ore -> 2 ore`. `4 clay robots +4 clay -> 11 clay`. `1 obsidian robot +1 obsidian -> 2 obsidian`.
1. `1 ore robot +1 ore -> 3 ore`. `4 clay robots +4 clay -> 15 clay`. `1 obsidian robot +1 obsidian -> 3 obsidian`.
1. Spend 3 ore and 14 clay to start building an obsidian robot. `1 ore robot +1 ore -> 1 ore`. `4 clay robots +4 clay -> 5 clay`. `1 obsidian robot +1 obsidian -> 4 obsidian`. The new obsidian robot ready -> 2 total.
1. `1 ore robot +1 ore -> 2 ore`. `4 clay robots +4 clay -> 9 clay`. `2 obsidian robots +2 obsidian -> 6 obsidian`.
1. `1 ore robot +1 ore -> 3 ore`. `4 clay robots +4 clay -> 13 clay`. `2 obsidian robots +2 obsidian -> 8 obsidian`.
1. Spend 2 ore and 7 obsidian to start building a geode robot. `1 ore robot +1 ore -> 2 ore`. `4 clay robots +4 clay -> 17 clay`. `2 obsidian robots +2 obsidian -> 3 obsidian`. The new geode robot is ready -> 1 total.
1. `1 ore robot +1 ore -> 3 ore`. `4 clay robots +4 clay -> 21 clay`. `2 obsidian robots +2 obsidian -> 5 obsidian`. `1 geode robot +1 geode -> 1 open geode`.
1. `1 ore robot +1 ore -> 4 ore`. `4 clay robots +4 clay -> 25 clay`. `2 obsidian robots +2 obsidian -> 7 obsidian`. `1 geode robot +1 geode -> 2 open geodes`.
1. Spend 2 ore and 7 obsidian to start building a geode robot. `1 ore robot +1 ore -> 3 ore`. `4 clay robots +4 clay -> 29 clay`. `2 obsidian robots +2 obsidian -> 2 obsidian`. `1 geode robot +1 geode -> 3 open geodes`. The new geode robot is ready -> 2 total.
1. `1 ore robot +1 ore -> 4 ore`. `4 clay robots +4 clay -> 33 clay`. `2 obsidian robots +2 obsidian -> 4 obsidian`. `2 geode robots +2 geodes -> 5 open geodes`.
1. `1 ore robot +1 ore -> 5 ore`. `4 clay robots +4 clay -> 37 clay`. `2 obsidian robots +2 obsidian -> 6 obsidian`. `2 geode robots +2 geodes -> 7 open geodes`.
1. `1 ore robot +1 ore -> 6 ore`. `4 clay robots +4 clay -> 41 clay`. `2 obsidian robots +2 obsidian -> 8 obsidian`. `2 geode robots +2 geodes -> 9 open geodes`.

However, by using blueprint 2 in the example above, you could do even better: the largest number of geodes you could open in 24 minutes is 12.

Determine the quality level of each blueprint by multiplying that blueprint's ID number with the largest number of geodes that can be opened in 24 minutes using that blueprint. In this example, the first blueprint has ID 1 and can open 9 geodes, so its quality level is 9. The second blueprint has ID 2 and can open 12 geodes, so its quality level is 24. Finally, if you add up the quality levels of all of the blueprints in the list, you get 33.



---

### Part 2


### Example



___


