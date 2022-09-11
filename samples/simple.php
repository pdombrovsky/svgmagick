<?php

use SvgMagick\Options;
use SvgMagick\Vectorizer;

require_once('functions.php');

/* This file illustrates simple (black/white) vectorization.
   Example: php simple.php -f path/to/image.jpg -t 0.55 -a
   -t       threshold value (0.0 < t < 1.0);
   -a       if the parameter is present, ignoreAlpha is false
   Expected output is Svg file: path/to/image.svg
*/
$args = getopt("f:t:a");
if (!$args) {
    echo 'An error occured while getting options or empty options are given';
    return;
}
if (!is_numeric($args['t'])) {
    echo 'Threshold is not numeric';
    return;
}
$ignoreAlpha = !key_exists('a', $args);
$image = new \Imagick();
try {
    $res = $image->readImage($args['f']);
    if (!$res) {
        throw new \Exception("Unable to read source file");
    }
    $options = new Options();
    /* The instance of 'Options' class contains default settings now, you can change them using accessor methods.
       See phpinfo() (or php -i) for details. */
    //$options->setAlphaMax(0.9);
    $vectorizer = new Vectorizer($options);
    /* Pixels with a gray level below the threshold are considered black, the rest are white.
       the gray level for each pixel is calculated as the average of the three color components (r + g + b) / 3
       or as the average of the color components and the alpha channel (r + g + b + a) / 4 , if the ignoreAlpha parameter is false.
    */
    $vectorizer->createSimpleBitmap($image, (float)$args['t'], $ignoreAlpha);
    $success = $vectorizer->trace();
    if (!$success) {
        throw new \Exception("An error occured while tracing bitmap");
    }
    $d = $vectorizer->getAttributeContent();
    if (!$d) {
        throw new \Exception("An error occured while converting path");
    }
    $width = $image->getImageWidth();
    $height = $image->getImageHeight();
    $content = getSvgContent($width, $height, $d);
    $svgFilename = getSvgFilename($args['f']);
    file_put_contents($svgFilename, $content);
    echo 'Succesfully completed';
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
} finally {
    $image->clear();
    $image->destroy();
}