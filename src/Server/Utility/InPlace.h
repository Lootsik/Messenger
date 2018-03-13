#pragma once

// do not allow copy or move
class InPlace 
{
public:
	InPlace() = default;
	~InPlace() = default;

	InPlace(InPlace&) = delete;
	InPlace& operator=(InPlace&) = delete;

	InPlace(InPlace&&) = delete;
	InPlace& operator=(InPlace&&) = delete;
};