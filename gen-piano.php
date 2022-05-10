<?php
////////////////////////////////////////////////////////////////////////////////////////////
const PPI = 300;

const PADDING = 10;
const KEYS_PADDING = 3;

const TAIL_HEIGHT = 8;

const KEY_WIDTH = 13;
const KEY_HEIGHT = 66;

const BKEY_WIDTH = KEY_WIDTH / 1.5;
const BKEY_HEIGHT = KEY_HEIGHT / 2 - KEYS_PADDING;
const BKEY_HEIGHT2 = 7;

const DRILL = 1.5;
////////////////////////////////////////////////////////////////////////////////////////////

$piano_size = $argv[1] ?? 25;
$piano_sizes = [13, 25, 37, 49, 61, 76, 88];

if (!in_array($piano_size, $piano_sizes))
	die("Unknown piano size! Available: ".implode(", ", array_keys($piano_sizes))."\n");

drawWhiteKeys($piano_size, false);
drawWhiteKeys($piano_size, true);
drawBlackKeys($piano_size);

function drawBlackKeys($piano_size) {
	$cnt = floor($piano_size / 12) * 5;
	
	if ($piano_size == 88 || $piano_size == 76)
		$total_keys++;
	
	$width = px(PADDING) * 2 + (px(BKEY_WIDTH) - px(KEYS_PADDING)) * $cnt;
	$height = px(PADDING) * 2 + px(BKEY_HEIGHT) + px(TAIL_HEIGHT) * 2 + px(BKEY_HEIGHT2) * 2;
	
	$img = imagecreatetruecolor($width, $height);
	
	$white = imagecolorallocate($img, 0xFF, 0xFF, 0xFF);
	imagefill($img, 0, 0, $white);
	
	$x = px(PADDING);
	$y = px(PADDING);
	
	for ($i = 0; $i < $cnt; $i++)
		$x += drawBKey($img, $x, $y);
	
	$file = "/tmp/piano-black-keys-for-cut.png";
	
	echo "$file\n";
	
	imagepng($img, $file);
}

function drawWhiteKeys($piano_size, $for_drill) {
	$cnt = floor($piano_size / 12);
	$total_keys = $cnt * 7 + 1;
	
	if ($piano_size == 88) {
		$total_keys += 2;
	} elseif ($piano_size == 76) {
		$total_keys += 9;
	}
	
	$width = px(PADDING) * 2 + ($total_keys * (px(KEY_WIDTH) + ($for_drill ? px(KEYS_PADDING) : 0)));
	$height = px(PADDING) * 2 + px(KEY_HEIGHT) + ($for_drill ? 0 : px(TAIL_HEIGHT) * 2);
	
	$img = imagecreatetruecolor($width, $height);
	
	$white = imagecolorallocate($img, 0xFF, 0xFF, 0xFF);
	imagefill($img, 0, 0, $white);
	
	$x = px(PADDING);
	$y = px(PADDING);
	
	$fix = px(KEY_WIDTH) * 0.1;
	$fix2 = px(KEY_WIDTH) * 0.15;
	
	if ($piano_size == 88) {
		$x += drawKey($img, $x, $y, 1, 0, $fix, $for_drill);
		$x += drawKey($img, $x, $y, 2, -$fix, 0, $for_drill);
	} elseif ($piano_size == 76) {
		$x += drawKey($img, $x, $y, 0, 0, 0, $for_drill);
		
		$x += drawKey($img, $x, $y, 1, 0, $fix2, $for_drill);
		$x += drawKey($img, $x, $y, 3, $fix2, 0, $for_drill);
		$x += drawKey($img, $x, $y, 3, 0, -$fix2, $for_drill);
		$x += drawKey($img, $x, $y, 2, -$fix2, 0, $for_drill);
	}
	
	for ($i = 0; $i < $cnt; $i++) {
		$x += drawKey($img, $x, $y, 1, 0, $fix, $for_drill);
		$x += drawKey($img, $x, $y, 3, $fix, -$fix, $for_drill);
		$x += drawKey($img, $x, $y, 2, -$fix, 0, $for_drill);
		
		$x += drawKey($img, $x, $y, 1, 0, $fix2, $for_drill);
		$x += drawKey($img, $x, $y, 3, $fix2, 0, $for_drill);
		$x += drawKey($img, $x, $y, 3, 0, -$fix2, $for_drill);
		$x += drawKey($img, $x, $y, 2, -$fix2, 0, $for_drill);
	}
	
	if ($piano_size == 76) {
		$x += drawKey($img, $x, $y, 1, 0, $fix, $for_drill);
		$x += drawKey($img, $x, $y, 3, $fix, -$fix, $for_drill);
		$x += drawKey($img, $x, $y, 2, -$fix, 0, $for_drill);
		
		$x += drawKey($img, $x, $y, 1, 0, $fix, $for_drill);
		$x += drawKey($img, $x, $y, 2, -$fix, 0, $for_drill);
	} else {
		$x += drawKey($img, $x, $y, 0, 0, 0, $for_drill);
	}
	
	$file = $for_drill ? "/tmp/piano-drill-map.png" : "/tmp/piano-white-keys-for-cut.png";
	
	echo "$file\n";
	
	imagepng($img, $file);
}

function drawBKey($img, $x, $y) {
	$black = imagecolorallocate($img, 0, 0, 0);
	
	$height2 = px(BKEY_HEIGHT2);
	$height = px(BKEY_HEIGHT) + $height2 * 2;
	$width = px(BKEY_WIDTH - KEYS_PADDING);
	
	$tail = px(DRILL);
	$half_tail = (($width - $tail) / 2);
	$tail_length = px(TAIL_HEIGHT);
	
	drawPoints($img, [
		[$x, $y + $tail_length],
		[$x + $width, $y + $tail_length]
	], $black);
	
	drawPoints($img, [
		[$x, $y + $height + $tail_length],
		[$x + $width, $y + $height + $tail_length]
	], $black);
	
	drawPoints($img, [
		[$x, $y + $height2 + $tail_length],
		[$x + $width, $y + $height2 + $tail_length]
	], $black);
	
	drawPoints($img, [
		[$x, $y + $height + $tail_length - $height2],
		[$x + $width, $y + $height + $tail_length - $height2]
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

function drawKey($img, $x, $y, $pad, $fix_l = 0, $fix_r = 0, $for_drill) {
	$black = imagecolorallocate($img, 0, 0, 0);
	
	$pad_r_w = px(BKEY_WIDTH) - $fix_r;
	$pad_r_h = px(BKEY_HEIGHT + KEYS_PADDING);
	
	$pad_l_w = px(KEY_WIDTH) - px(BKEY_WIDTH) - $fix_l;
	$pad_l_h = px(KEY_HEIGHT) - (px(BKEY_HEIGHT + KEYS_PADDING));
	
	$tail = px(DRILL);
	$tail_length = px(TAIL_HEIGHT);
	
	if ($for_drill)
		imageellipse($img, $x + px(KEY_WIDTH / 2), $y + px(KEY_HEIGHT), px(DRILL), px(DRILL), $black);
	
	if ($pad == 1) {
		if ($for_drill) {
			imageellipse($img, $x + ($pad_r_w / 2), $y, px(DRILL), px(DRILL), $black);
			
			$bk_x = $x + $pad_r_w + px(KEYS_PADDING);
			$bk_w = px(BKEY_WIDTH - KEYS_PADDING);
			$bk_h = px(BKEY_HEIGHT);
			
			drawPoints($img, [
				[$bk_x, $y],
				[$bk_x, $y +$bk_h],
				[$bk_x + $bk_w, $y + $bk_h],
				[$bk_x + $bk_w, $y],
				[$bk_x, $y],
			], $black);
			
			imageellipse($img, $bk_x + $bk_w / 2, $y, px(DRILL), px(DRILL), $black);
			imageellipse($img, $bk_x + $bk_w / 2, $y + $bk_h, px(DRILL), px(DRILL), $black);
		} else {
			$half_tail = ($pad_r_w - $tail) / 2;
			
			drawPoints($img, [
				[$x + $half_tail, $y],
				[$x + $half_tail + $tail, $y],
				[$x + $half_tail + $tail, $y + $tail_length],
				[$x + $half_tail, $y + $tail_length],
				[$x + $half_tail, $y],
			], $black);
			
			$y += $tail_length;
		}
		
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
		if ($for_drill) {
			imageellipse($img, $x + $pad_l_w + ((px(KEY_WIDTH) - $pad_l_w) / 2), $y, px(DRILL), px(DRILL), $black);
		} else {
			$half_tail = $pad_l_w + (((px(KEY_WIDTH) - $pad_l_w) - $tail) / 2);
			
			drawPoints($img, [
				[$x + $half_tail, $y],
				[$x + $half_tail + $tail, $y],
				[$x + $half_tail + $tail, $y + $tail_length],
				[$x + $half_tail, $y + $tail_length],
				[$x + $half_tail, $y],
			], $black);
			
			$y += $tail_length;
		}
		
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
		if ($for_drill) {
			imageellipse($img, $x + $pad_l_w + (($pad_r_w - $pad_l_w) / 2), $y, px(DRILL), px(DRILL), $black);
			
			$bk_x = $x + $pad_r_w + px(KEYS_PADDING);
			$bk_w = px(BKEY_WIDTH - KEYS_PADDING);
			$bk_h = px(BKEY_HEIGHT);
			
			drawPoints($img, [
				[$bk_x, $y],
				[$bk_x, $y +$bk_h],
				[$bk_x + $bk_w, $y + $bk_h],
				[$bk_x + $bk_w, $y],
				[$bk_x, $y],
			], $black);
			
			imageellipse($img, $bk_x + $bk_w / 2, $y, px(DRILL), px(DRILL), $black);
			imageellipse($img, $bk_x + $bk_w / 2, $y + $bk_h, px(DRILL), px(DRILL), $black);
		} else {
			$half_tail = $pad_l_w + ((($pad_r_w - $pad_l_w) - $tail) / 2);
			
			drawPoints($img, [
				[$x + $half_tail, $y],
				[$x + $half_tail + $tail, $y],
				[$x + $half_tail + $tail, $y + $tail_length],
				[$x + $half_tail, $y + $tail_length],
				[$x + $half_tail, $y],
			], $black);
			
			$y += $tail_length;
		}
		
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
		if ($for_drill) {
			imageellipse($img, $x + px(KEY_WIDTH / 2), $y, px(DRILL), px(DRILL), $black);
		} else {
			$half_tail = ((px(KEY_WIDTH) - $tail) / 2);
			
			drawPoints($img, [
				[$x + $half_tail, $y],
				[$x + $half_tail + $tail, $y],
				[$x + $half_tail + $tail, $y + $tail_length],
				[$x + $half_tail, $y + $tail_length],
				[$x + $half_tail, $y],
			], $black);
			
			$y += $tail_length;
		}
		
		drawPoints($img, [
			[$x, $y],
			[$x, $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y + px(KEY_HEIGHT)],
			[$x + px(KEY_WIDTH), $y],
			[$x, $y]
		], $black);
	}
	
	if (!$for_drill) {
		$half_tail = ((px(KEY_WIDTH) - $tail) / 2);
		
		drawPoints($img, [
			[$x + $half_tail, $y + px(KEY_HEIGHT)],
			[$x + $half_tail + $tail, $y + px(KEY_HEIGHT)],
			[$x + $half_tail + $tail, $y + $tail_length + px(KEY_HEIGHT)],
			[$x + $half_tail, $y + $tail_length + px(KEY_HEIGHT)],
			[$x + $half_tail, $y + px(KEY_HEIGHT)],
		], $black);
	}
	
	return px(KEY_WIDTH) + ($for_drill ? px(KEYS_PADDING) : 0);
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
