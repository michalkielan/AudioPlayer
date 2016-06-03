/*
 * Player.h
 *
 *  Created on: Jun 3, 2016
 *      Author: michal
 */

#ifndef PLAYER_H_
#define PLAYER_H_

namespace Audio {

class Player {

public:
	Player();

	virtual void play(const char* _filename) = 0;

	virtual ~Player();
};

} /* namespace Sound */

#endif /* PLAYER_H_ */
