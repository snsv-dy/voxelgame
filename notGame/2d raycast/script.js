// Source of explanation of this algorithm.
// https://www.youtube.com/watch?v=NbSee-XM7WA
//
// Source of algorithm from that video.
// https://lodev.org/cgtutor/raycasting.html
//
// Y axis is inverted in this program, but it doesn't look like it matter (Its mirrored).
//
// AABB version inspired by https://github.com/andyhall/voxel-aabb-sweep


const gridSize = 20;
var grid = null;
var canvas = null;
var c = null;
var cellSize = null;
const floor = Math.floor;

var aabb = makeAABB(0, 0, 0.9, 1.8);
var origin = { x: -1, y: -1 };
var relativeOrigin = { x: 0, y: 0 };
var target = { x: -1, y: -1 };
var translation = { x: -1, y: -1 }; // Translation in pixels.

window.onload = function (e) {
	initGrid();
	initCanvas();
	drawGrid();
	canvas.addEventListener('mousemove', mouseMoved);
	canvas.addEventListener('click', modifyTerrain);
	window.addEventListener('keydown', keyDown);
}

function keyDown(e) {
	switch (e.code) {
		case 'KeyW':
			moveOrigin(0, -1);
			break;
		case 'KeyS':
			moveOrigin(0, 1);
			break;
		case 'KeyA':
			moveOrigin(-1, 0);
			break;
		case 'KeyD':
			moveOrigin(1, 0);
			break;
	}
	draw();
}

function makeAABB(x, y, w, h) {
	return {
		min: { x: x, y: y },
		max: { x: x + w, y: y + h },
		width: w,
		height: h
	};
}

function translateAABB(aabb, x, y) {
	x -= translation.x;
	y -= translation.y;

	var dx = x / cellSize - aabb.min.x;
	var dy = y / cellSize - aabb.min.y;

	aabb.min.x += dx;
	aabb.min.y += dy;

	aabb.max.x += dx;
	aabb.max.y += dy;
}

function moveOrigin(x, y) {
	const speed = 0.2;
	relativeOrigin.x += x * speed;
	relativeOrigin.y += y * speed;

	var ox = relativeOrigin.x > 0 ? aabb.max.x : aabb.min.x;
	var oy = relativeOrigin.y > 0 ? aabb.max.y : aabb.min.y;

	origin.x = relativeOrigin.x + ox;
	origin.y = relativeOrigin.y + oy;
}

function updateTarget(x, y) {
	x -= translation.x;
	y -= translation.y;
	
	var ox = relativeOrigin.x > 0 ? aabb.max.x : aabb.min.x;
	var oy = relativeOrigin.y > 0 ? aabb.max.y : aabb.min.y;

	// target.x = x / cellSize + ox;
	// target.y = y / cellSize + oy;
	target.x = ox;
	target.y = oy;
}


function modifyTerrain(e) {
	if (e.button == 0) {
		// Target is updated in mouseMoved.
		var ix = floor(target.x);
		var iy = floor(target.y);

		ix += floor(translation.x / cellSize);
		iy += floor(translation.x / cellSize);

		grid[iy][ix] = 1 - grid[iy][ix];
	}
}

function mouseMoved(e) {
	if (e.button == 0) {
		translateAABB(aabb, e.offsetX, e.offsetY);
		moveOrigin(0, 0);
		updateTarget(e.offsetX, e.offsetY);
		draw();
	}
}

function draw() {
	c.clearRect(0, 0, canvas.width, canvas.height);
	drawGrid();
	drawPoints();
	drawAABB();
	let collisions = castAABB(aabb, origin, target);
	if (collisions !== -1) {
		for (let collision of collisions) {
			drawCollisionVoxel(collision.voxel);
			drawCollisionPoint(collision.point);
		}
	}
}

// This is what you are looking for.
function castRay(origin, target) {
	var direction = { x: target.x - origin.x, y: target.y - origin.y };

	// Normalize direction
	var direction_len = Math.sqrt(direction.x * direction.x + direction.y * direction.y);
	direction.x /= direction_len;
	direction.y /= direction_len;

	// Scaling factors
	var sx = Math.sqrt(1 + Math.pow(direction.y / direction.x, 2));
	var sy = Math.sqrt(1 + Math.pow(direction.x / direction.y, 2));

	// Distance traveled in axis
	var traveledx = direction.x >= 0 ? floor(origin.x) + 1 - origin.x : origin.x - floor(origin.x);
	var traveledy = direction.y >= 0 ? floor(origin.y) + 1 - origin.y : origin.y - floor(origin.y);

	// Change value remaining to get to the next voxel, to distance traveled along the ray.
	traveledx *= sx;
	traveledy *= sy;

	// Voxel index in grid.
	var ix = floor(origin.x);
	var iy = floor(origin.y);

	// Direction in which voxel indicies will be incremented.
	var stepx = direction.x > 0 ? 1 : -1;
	var stepy = direction.y > 0 ? 1 : -1;

	var t = 0.0;
	var collisions = [];
	while (t < direction_len) {

		if (checkSolid(ix, iy)) {
			c.beginPath();
			c.fillText("" + ix + ", " + iy, 10, 10);
			collisions.push({
				voxel: { x: ix, y: iy },
				point: { x: origin.x + t * direction.x, y: origin.y + t * direction.y }
			});
			break;
			// return {
			// 	// 	voxel: { x: ix, y: iy },
			// 	// 	point: { x: origin.x + t * direction.x, y: origin.y + t * direction.y }
			// 	// };
		}

		if (traveledx < traveledy) {
			t = traveledx; // Bery important! (Set t before adding sx to traveled)
			traveledx += sx;
			ix += stepx;
		} else {
			t = traveledy;
			traveledy += sy;
			iy += stepy;
		}
	}

	if (collisions.length == 0) {
		return -1;
	}

	return collisions;
}

function castAABB(aabb, origin, target) {

	// Direction in which voxel indicies will be incremented.
	// (Change to find the closest)
	// var stepx = direction.x > 0 ? 1 : -1;
	// var stepy = direction.y > 0 ? 1 : -1;

	var direction = { x: origin.x - target.x, y: origin.y - target.y };

	var stepx = direction.x > 0 ? 1 : -1;
	var stepy = direction.y > 0 ? 1 : -1;

	var origin = {
		x: stepx == 1 ? aabb.max.x : aabb.min.x,
		y: stepy == 1 ? aabb.max.y : aabb.min.y
	}

	// Normalize direction
	var direction_len = Math.sqrt(direction.x * direction.x + direction.y * direction.y);
	direction.x /= direction_len;
	direction.y /= direction_len;

	// Scaling factors
	var sx = Math.sqrt(1 + Math.pow(direction.y / direction.x, 2));
	var sy = Math.sqrt(1 + Math.pow(direction.x / direction.y, 2));

	// Voxel index in grid.
	// var ix = floor(origin.x);
	// var iy = floor(origin.y);
	var ileadx = stepx == 1 ? floor(aabb.max.x) : floor(aabb.min.x);
	var ileady = stepy == 1 ? floor(aabb.max.y) : floor(aabb.min.y);

	var itrailx = stepx == 1 ? floor(aabb.min.x) : floor(aabb.max.x);
	var itraily = stepy == 1 ? floor(aabb.min.y) : floor(aabb.max.y);

	// Distance traveled in axis
	var traveledx = direction.x >= 0 ? floor(origin.x) + 1 - origin.x : origin.x - floor(origin.x);
	var traveledy = direction.y >= 0 ? floor(origin.y) + 1 - origin.y : origin.y - floor(origin.y);

	// Change value remaining to get to the next voxel, to distance traveled along the ray.
	traveledx *= sx;
	traveledy *= sy;

	var axis = traveledx < traveledy ? 0 : 1;
	var t = 0.0;
	var collisions = [];
	while (t < direction_len) {
		// Collision checking.
		var ybeg = axis == 1 ? ileady : itraily;
		var xbeg = axis == 0 ? ileadx : itrailx;

		var yend = ileady + stepy;
		var xend = ileadx + stepx;

		// if (t < 0.001) {
		// 	console.log(ybeg, yend);
		// 	console.log(xbeg, xend);
		// }

		c.beginPath();
		c.fillText("min" + aabb.min.x + ", " + aabb.min.y, 10, 60);
		c.fillText("max" + aabb.max.x + ", " + aabb.max.y, 10, 70);

		var loops = 0;
		var out = false; // To get out of outer loop;
		for (var y = ybeg; !out && y != yend; y += stepy) {
			for (var x = xbeg; x != xend; x += stepx) {
				loops++;
				if (loops > 20) {
					out = true;
					break;
				}
				if (checkSolid(x, y)) {
					c.beginPath();
					c.fillText("beg" + xbeg + ", " + ybeg, 10, 10);
					c.fillText("end" + xend + ", " + yend, 10, 20);
					
					// c.beginPath();
					c.fillText("lead" + ileadx + ", " + ileady, 10, 40);
					c.fillText("trai" + itrailx + ", " + itraily, 10, 50);
					c.fillText("t " + t + ", axis" + axis, 200, 10);
					// console.log(JSON.stringify({
					// 	voxel: { x: x, y: y },
					// 	point: { x: origin.x + t * direction.x, y: origin.y + t * direction.y }
					// }));
					collisions.push({
						voxel: { x: x, y: y },
						point: { x: origin.x + t * direction.x, y: origin.y + t * direction.y }
					});
					out = true;
					break;
				}
			}
		}

		if(out){
			break;
		}
		// End of collision checking.

		if (traveledx < traveledy) {
			t = traveledx; // Bery important! (Set t before adding sx to traveled)
			traveledx += sx;
			ileadx += stepx;
			itrailx += stepx;
			axis = 0;
		} else {
			t = traveledy;
			traveledy += sy;
			ileady += stepy;
			itraily += stepy;
			axis = 0;
		}
	}

	if (collisions.length == 0) {
		return -1;
	}

	return collisions;
}

function checkSolid(x, y) {
	x += floor(translation.x / cellSize);
	y += floor(translation.x / cellSize);
	// console.log(x, y);
	return grid[y][x] == 1;
}

function drawCollisionVoxel(collision) {
	c.beginPath();
	c.strokeStyle = 'pink';
	c.rect(collision.x * cellSize + translation.x, collision.y * cellSize + translation.y, cellSize, cellSize);
	c.lineWidth = 3;
	c.stroke();
	c.lineWidth = 1;
}


function drawCollisionPoint(point) {
	c.beginPath();
	c.strokeStyle = 'orange';
	// console.log(point.x * cellSize, point.y * cellSize);
	// c.rect(collision.x * cellSize, collision.y * cellSize, cellSize, cellSize);
	c.arc(point.x * cellSize + translation.x, point.y * cellSize + translation.y, 3, 0, 2 * Math.PI);
	c.lineWidth = 3;
	c.stroke();
	c.lineWidth = 1;
}

function drawPoints() {
	c.beginPath();
	c.moveTo(origin.x * cellSize + translation.x, origin.y * cellSize + translation.y);
	c.lineTo(target.x * cellSize + translation.x, target.y * cellSize + translation.y);
	c.strokeStyle = 'black';
	c.stroke();

	c.beginPath();
	c.arc(origin.x * cellSize + translation.x, origin.y * cellSize + translation.y, 5, 0, 2 * Math.PI);
	c.fillStyle = 'blue';
	c.fill();

	c.beginPath();
	c.arc(target.x * cellSize + translation.x, target.y * cellSize + translation.y, 5, 0, 2 * Math.PI);
	c.fillStyle = 'red';
	c.fill();
}

function drawAABB() {
	c.beginPath();
	// c.arc(origin.x * cellSize + translation.x, origin.y * cellSize + translation.y, 5, 0, 2 * Math.PI);
	c.rect(aabb.min.x * cellSize + translation.x, aabb.min.y * cellSize + translation.y, aabb.width * cellSize, aabb.height * cellSize);
	c.strokeStyle = 'blue';
	c.stroke();
}

function drawGrid() {
	c.strokeStyle = 'gray';

	var xTranslate = translation.x;
	var y

	// Drawing vertical lines.
	for (var x = 0; x < gridSize + 1; x++) {
		c.beginPath();
		c.moveTo(x * cellSize, 0);
		c.lineTo(x * cellSize, canvas.height);
		c.stroke();
	}

	// Drawing horizontal lines.
	for (var y = 0; y < gridSize + 1; y++) {
		c.beginPath();
		c.moveTo(0, y * cellSize);
		c.lineTo(canvas.width, y * cellSize);
		c.stroke();
	}

	// Drawing filled cells.
	c.fillStyle = 'green';
	for (var y = 0; y < gridSize; y++) {
		for (var x = 0; x < gridSize; x++) {
			if (grid[y][x] != 0) {
				c.beginPath();
				c.rect(x * cellSize, y * cellSize, cellSize, cellSize);
				c.fill();
			}
		}
	}
}

function initCanvas() {
	canvas = document.createElement('canvas');
	canvas.width = 500;
	canvas.height = 500;
	document.querySelector('div').appendChild(canvas);
	c = canvas.getContext('2d');
	cellSize = canvas.width / gridSize;
	origin.x = origin.y = 0;//canvas.width / 2 / cellSize - 0.1;
	translation.x = canvas.width / 2;
	translation.y = canvas.height / 2;
}

function initGrid() {
	const w = gridSize;
	const h = gridSize;

	grid = [];
	for (var y = 0; y < h; y++) {
		grid[y] = [];
		for (var x = 0; x < h; x++) {
			grid[y][x] = terrain(x, y);
		}
	}
}

function terrain(x, y) {
	const rx = 5, ry = 5, w = 9, h = 9;
	if (((x == rx || x == rx + w) && y >= ry && y <= ry + h) ||
		((y == ry || y == ry + w) && x >= rx && x < rx + h)) {
		return 1;
	}

	return 0;
}