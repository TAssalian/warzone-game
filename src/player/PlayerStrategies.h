#pragma once

#include <iostream>
#include <string>
#include <vector>

class Player;
class Territory;

// Abstract Strategy class
class PlayerStrategy {
public:
  virtual ~PlayerStrategy() = default;

  virtual void issueOrder(const Player *player) = 0;
  virtual std::vector<Territory *> toAttack(const Player *player) const = 0;
  virtual std::vector<Territory *> toDefend(const Player *player) const = 0;

  virtual std::string getStrategyName() const = 0;

  friend std::ostream &operator<<(std::ostream &os, const PlayerStrategy &ps);
};

// Human player: requires user interactions to make decisions
class HumanPlayerStrategy : public PlayerStrategy {
public:
  void issueOrder(const Player *player) override;
  std::vector<Territory *> toAttack(const Player *player) const override;
  std::vector<Territory *> toDefend(const Player *player) const override;
  std::string getStrategyName() const override;
};

// Aggressive player: focuses on attack
// Deploys/advances armies on its strongest country,
// then always advances to enemy territories until it cannot do so anymore
class AggressivePlayerStrategy : public PlayerStrategy {
public:
  void issueOrder(const Player *player) override;
  std::vector<Territory *> toAttack(const Player *player) const override;
  std::vector<Territory *> toDefend(const Player *player) const override;
  std::string getStrategyName() const override;
};

// Benevolent player: focuses on protecting weak countries
// Deploys/advances armies on its weakest countries,
// never advances to enemy territories
class BenevolentPlayerStrategy : public PlayerStrategy {
public:
  void issueOrder(const Player *player) override;
  std::vector<Territory *> toAttack(const Player *player) const override;
  std::vector<Territory *> toDefend(const Player *player) const override;
  std::string getStrategyName() const override;
};

// Neutral player: never issues any order
// If attacked, becomes an Aggressive player
class NeutralPlayerStrategy : public PlayerStrategy {
public:
  void issueOrder(const Player *player) override;
  std::vector<Territory *> toAttack(const Player *player) const override;
  std::vector<Territory *> toDefend(const Player *player) const override;
  std::string getStrategyName() const override;
};

// Cheater player: automatically conquers all adjacent enemy territories (once per turn)
class CheaterPlayerStrategy : public PlayerStrategy {
public:
  void issueOrder(const Player *player) override;
  std::vector<Territory *> toAttack(const Player *player) const override;
  std::vector<Territory *> toDefend(const Player *player) const override;
  std::string getStrategyName() const override;
};
