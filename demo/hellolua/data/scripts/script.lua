
timePassed = 0.0

function update(t)
	timePassed = timePassed + t
	return math.sin(timePassed);
end