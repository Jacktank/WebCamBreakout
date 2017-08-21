# WebCamBreakout

For this project we built a well-known game "Breakout", released by Atari Inc. in 19776, which is played with the movement of our hand. In the game, the top of the screen is covered by several blocks and there is a ball that protrudes on the walls, Blocks and on the platform. Whenever a block is hit by the ball. The player loses when the ball leaves the bottom of the game screen. To prevent this from happening, the player has a mobile platform to bring the ball back to the top of the screen. It is this platform that is controllable by the movement of our hand. The image of our hand is captured by a normal webcam and through the library OpenCV we developed an algorithm that fundamentally recognizes the palm of our hand and associates its position in the real world with Position of the platform in the game. We tried several techniques for hand detection. We started by using a background subtraction algorithm where the background is removed to extract the points of interest from the hand. We then decided to use a technique where we used predefined ranges of hue and saturation with which we calculated a histogram. Because we did not obtain good results in the implementation of these techniques we used ranges of dynamic values with Back Projection, which is a way to record how well The pixels of an image fit the distribution of pixels in a histogram. That is, we calculate the histogram of an area that interests us in the image and we use it to find this area in another image.We have this technique to recognize the hand in real time and without lagnotávelo that allows us a good interaction with the game.

![Screen 1](https://github.com/luispmenezes/BallBlaster/blob/master/img_1.png)

[Video Demo](https://drive.google.com/open?id=0B3nMgJHBhW_QXzhvZkdqZVNxb28)