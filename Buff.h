#ifndef BUFF_H
#define BUFF_H

class BuffBase
{
public:
	BuffBase();
	~BuffBase();

private:
	Buff_Tag tag;
	int duration; // how many turns the buff will last
protected:
	//what will happen when the buff is applied, such as applying damage over time, increasing stats, etc.
	virtual void OnBuffGet();
	// what will happen every turn when the buff is active, such as applying damage over time, decreasing stats, etc.
	virtual void OnBuffTurnUpdate(double elapsed_time);
	// what will happen when the buff ends, such as removing stat increases, etc.
	virtual void OnBuffEnd();
};

BuffBase::BuffBase()
{
}

BuffBase::~BuffBase()
{
}

// Buff_Tag is used to identify the type of buff, such as "poison", "stun", "heal over time", etc.
enum class Buff_Tag
{

};

#endif
