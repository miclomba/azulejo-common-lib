
#include "config.h"

#include <memory>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include "Entities/IRunnableEntity.h"

namespace
{
const int LARGE_INT = 2000;

int SumSquares(const int x)
{
	int accum = 0;
	for (int i = 0; i <= x; ++i)
		accum += i * i;
	return accum;
}

class Runnable : public entity::IRunnableEntity
{
public:
	Runnable(const int x) : x_(x) { accum_ = 0; };
	int GetAccum() { return accum_; }

protected:
	void Run() override { accum_ += x_ * x_; };

private:
	int x_;
	static int accum_;
};

int Runnable::accum_ = 0;
} // end namespace anonymous

TEST(IRunnableEntity, Start)
{
	const int x = 3;
	Runnable runn(x);

	EXPECT_NO_THROW(runn.Start());
	runn.Join();
	EXPECT_EQ(runn.GetAccum(), x*x);
}

TEST(IRunnableEntity, StartThrows)
{
	const int x = 3;
	Runnable runn(x);

	EXPECT_NO_THROW(runn.Start());
	EXPECT_THROW(runn.Start(), std::runtime_error);
}

TEST(IRunnableEntity, Join)
{
	std::vector<std::shared_ptr<Runnable>> runnables;

	for (int i = 0; i <= LARGE_INT; ++i)
		runnables.push_back(std::make_shared<Runnable>(i));

	for (std::shared_ptr<Runnable>& runn : runnables)
		runn->Start();

	for (std::shared_ptr<Runnable>& runn : runnables)
		EXPECT_NO_THROW(runn->Join());

	EXPECT_EQ(runnables[0]->GetAccum(), SumSquares(LARGE_INT));
}

TEST(IRunnableEntity, JoinThrows)
{
	const int x = 3;
	Runnable runn(x);

	EXPECT_THROW(runn.Join(), std::runtime_error);
}
