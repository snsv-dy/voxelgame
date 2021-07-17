const gridSize = 20;
var grid = null;
var canvas = null;
var c = null;
var cellSize = null;
const floor = Math.floor;

var origin = { x: -1, y: -1 };
var target = { x: -1, y: -1 };
var translation = { x: -1, y: -1 }; // Translation in pixels.

window.onload = function (e) {
	initGrid();
	initCanvas();
	drawGrid();
	canvas.addEventListener('mousemove', mouseMoved);
	canvas.addEventListener('click', modifyTerrain);
	window.addEventListener('keydown', keyDown);

	// delete later
	var chsize = 4;
	for (var i = 0; i > -10; i--) {
		var t = i % chsize;
		console.log(toNonsymetricSpace(i, chsize));
	}
}

function toNonsymetricSpace(value, chunkSize) {
	var chunk_index;
	var index_in_chunk;

	if (value >= 0) {
		chunk_index = value / chunkSize;
		index_in_chunk = value % chunkSize;
	} else {
		var mx = (-value);
		chunk_index = -(mx / chunkSize + 1);
		index_in_chunk = -(mx % chunkSize) + (chunkSize - 1);
	}

	return [chunk_index, index_in_chunk];
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

function moveOrigin(x, y) {
	const speed = 0.2;
	origin.x += x * speed;
	origin.y += y * speed;
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
		updateTarget(e.offsetX, e.offsetY);
		draw();
	}
}

function draw() {
	c.clearRect(0, 0, canvas.width, canvas.height);
	drawGrid();
	drawPoints();
	let collisions = castRay(origin, target);
	if (collisions !== -1) {
		for (let collision of collisions ) {
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

	function updateTarget(x, y) {
		x -= translation.x;
		y -= translation.y;
		target.x = x / cellSize;
		target.y = y / cellSize;
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