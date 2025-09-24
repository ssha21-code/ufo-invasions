# ufo-invasions
* This is a program made in Raylib C++. It is a small, fun game made in C++.
* The Player can move left and right, and can also jump.
* The Player carries an AK-47 with unlimited bullets.
* The Player has 50 HP.
* The Player aims the AK-47 to the mouse cursor.
* The controls of the game are:
  - Move left: Key A
  - Move right: Key D
  - Jump: Key Space
  - Shoot: Left Mouse Button
* Each round, three UFOs spawn and one of them can project their beam which reduces the Player's HP.
* Each UFO has 15 HP.
* The UFO which projects its beam tries to chase down and kill the Player.
* When the Player stands inside the UFO's beam, the Player takes damage every 0.5 seconds.
* The damage done by the UFO depends on the Player's count of Waves Beaten:
  - When the count is less than five, the UFO does damage from a random number between 1 and 3.
  - When the count is less than ten, the UFO does damage from a random number between 1 and 4.
  - When the count is more than ten, the UFO does damage from a random number between 1 and 5.
* The UFO must be shot down by the player, and one bullet does 1 HP of damage to the UFO.
* When the UFO that chases the player dies, the next UFO chases the player.
* When all the UFOs die, the Waves Beaten count increments, and shortly after, another wave spawns, then another.
* The Player's goal is to shoot down the UFOs with the right tactics, jump to try to get closer to the UFO and shoot, try to stay out of the UFO beam at all times, and to get a score (Waves Beaten) as high as possible while avoiding death.
* The user can play a series of rounds, and the highest count of Waves Beaten in each round is displayed on the home screen.
* Similar to the game fly-thru-boxes, I have included an executable with all the textures and sounds included in the executable file.
* There is also a video file of the gameplay inside the ufo-invasions folder.
* I hope all players have as much fun playing my game, just as how much fun I had designing it. 😉
