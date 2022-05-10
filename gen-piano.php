<?php
const PPI = 300;

const KEY_WIDTH = 13;
const KEY_HEIGHT = 66;

$width = px(297);
$height = px(210);

$img = imagecreatetruecolor($width, $height);

$white = imagecolorallocate($img, 0xFF, 0xFF, 0xFF);
$black = imagecolorallocate($img, 0, 0, 0);

imagefill($img, 0, 0, $white);

$x = px(10);
$y = px(10);

$fix = px(KEY_WIDTH) * 0.1;
$fix2 = px(KEY_WIDTH) * 0.15;

$x += drawKey($img, $x, $y, 1, 0, $fix);
$x += drawKey($img, $x, $y, 3, $fix, -$fix);
$x += drawKey($img, $x, $y, 2, -$fix, 0);
$x += drawKey($img, $x, $y, 1, 0, $fix2);
$x += drawKey($img, $x, $y, 3, $fix2, 0);
$x += drawKey($img, $x, $y, 3, 0, -$fix2);
$x += drawKey($img, $x, $y, 2, -$fix2, 0);

$x += drawKey($img, $x, $y, 1, 0, $fix);
$x += drawKey($img, $x, $y, 3, $fix, -$fix);
$x += drawKey($img, $x, $y, 2, -$fix, 0);
$x += drawKey($img, $x, $y, 1, 0, $fix2);
$x += drawKey($img, $x, $y, 3, $fix2, 0);
$x += drawKey($img, $x, $y, 3, 0, -$fix2);
$x += drawKey($img, $x, $y, 2, -$fix2, 0);

$x += drawKey($img, $x, $y, 0);

$y += px(KEY_HEIGHT) + px(10);
$x = px(10);

$x += drawBKey($img, $x, $y);
$x += drawBKey($img, $x, $y);
$x += drawBKey($img, $x, $y);
$x += drawBKey($img, $x, $y);
$x += drawBKey($img, $x, $y);
$x += drawBKey($img, $x, $y);
$x += drawBKey($img, $x, $y);
$x += drawBKey($img, $x, $y);
$x += drawBKey($img, $x, $y);
$x += drawBKey($img, $x, $y);

imagepng($img, "/tmp/xuj2.png");
rename("/tmp/xuj2.png", "/tmp/xuj.png");

echo date("Y-m-d H:i:s")."\n";

function drawBKey($img, $x, $y) {
	$black = imagecolorallocate($img, 0, 0, 0);
	
	$height2 = px(7);
	$height = px(KEY_HEIGHT) / 2 - px(4) + $height2 * 2;
	$width = px(KEY_WIDTH) - px(7);
	
	$tail = px(1.5);
	$half_tail = (($width - $tail) / 2);
	$tail_length = px(8);
	
	drawPoints($img, [
		[$x, $y + $height2 + $tail_length],
		[$x + $width, $y + $height2 + $tail_length]
	], $black);
	
	drawPoints($img, [
		[$x, $y + $height2 - $tail_length + $height],
		[$x + $width, $y + $height2 - $tail_length + $height]
	], $black);
	
	drawPoints($img, [
		[$x + $half_tail, $y],
		[$x + $half_tail, $y + $tail_length],
		[$x, $y + $tail_length],
		[$x, $y + $height + $tail_length],
		[$x + $half_tail, $y + $height + $tail_length],
		[$x + $half_tail, $y + $height + $tail_length * 2],
		[$x + $half_tail + $tail, $y + $height + $tail_length * 2],
		[$x + $half_tail + $tail, $y + $height + $tail_length],
		[$x + $width, $y + $height + $tail_length],
		[$x + $width, $y + $tail_length],
		[$x + $half_tail + $tail, $y + $tail_length],
		[$x + $half_tail + $tail, $y],
		[$x + $half_tail, $y],
	], $black);
	
	return $width;
}

function drawKey($img, $x, $y, $pad, $fix_l = 0, $fix_r = 0) {
	$black = imagecolorallocate($img, 0, 0, 0);
	
	$pad_r_w = px(KEY_WIDTH) / 1.5 - $fix_r;
	$pad_r_h = px(KEY_HEIGHT) / 2;
	
	$pad_l_w = px(KEY_WIDTH) - px(KEY_WIDTH) / 1.5 - $fix_l;
	$pad_l_h = px(KEY_HEIGHT) - px(KEY_HEIGHT) / 2;
	
	if ($pad == 1) {
		drawPoints($img, [
			[$x, $y],
			[$x, $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y + $pad_r_h],
			[$x + $pad_r_w, $y + $pad_r_h],
			[$x + $pad_r_w, $y],
			[$x, $y]
		], $black);
	} elseif ($pad == 2) {
		drawPoints($img, [
			[$x + $pad_l_w, $y],
			[$x + $pad_l_w, $y + $pad_l_h],
			[$x, $y + $pad_l_h],
			[$x, $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y],
			[$x + $pad_l_w, $y]
		], $black);
	} elseif ($pad == 3) {
		drawPoints($img, [
			[$x + $pad_l_w, $y],
			[$x + $pad_l_w, $y + $pad_l_h],
			[$x, $y + $pad_l_h],
			[$x, $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y + $pad_r_h],
			[$x + $pad_r_w, $y + $pad_r_h],
			[$x + $pad_r_w, $y],
			[$x + $pad_l_w, $y]
		], $black);
	} else {
		drawPoints($img, [
			[$x, $y],
			[$x, $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y],
			[$x, $y]
		], $black);
	}
	
	return px(KEY_WIDTH) + px(3);
}

function drawPoints($img, $points, $color) {
	$first = array_shift($points);
	$x = $first[0];
	$y = $first[1];
	
	foreach ($points as $point) {
		imageline($img, $x, $y, $point[0], $point[1], $color);
		$x = $point[0];
		$y = $point[1];
	}
}

function px($mm) {
	$ppmm = PPI / (2.54 * 10);
	return ceil($mm * $ppmm);
}
