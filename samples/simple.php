<?php

use SvgMagick\Options;
use SvgMagick\Vectorizer;

/* This file illustrates simple (black/white) vectorization.
   Example: php simple.php -f path/to/image.jpg -t 0.55 -a
   -t       threshold value (0.0 < t < 1.0);
   -a       if the parameter is present, ignoreAlpha is false
   Expected output is Svg file: path/to/image.svg
*/
const __SVG_ATTRS__ = array(
    'xmlns:rdf' => "http://www.w3.org/1999/02/22-rdf-syntax-ns#",
    'xmlns:svg' => "http://www.w3.org/2000/svg",
    'xmlns' => "http://www.w3.org/2000/svg",
    'xmlns:xlink' => "http://www.w3.org/1999/xlink",
    'namespace' => "http://www.w3.org/XML/1998/namespace",
    'version' => "1.1"
);
function getDomDocument(): \DOMDocument
{
    $doc = new \DOMDocument('1.0', 'UTF-8');
    $doc->preserveWhiteSpace = false;
    $doc->validateOnParse = true;
    $doc->formatOutput = true;
    return $doc;
}
function addSvg(\DOMNode $parent, float $width, float $height): \DOMElement
{
    $svg = new \DOMElement('svg');
    $parent->appendChild($svg);
    foreach (__SVG_ATTRS__ as $key => $value) {
        $svg->setAttribute($key, $value);
    }
    $svg->setAttribute('width', $width);
    $svg->setAttribute('height', $height);
    $svg->setAttribute('viewBox', "0 0 " . $width . " " . $height);
    return $svg;
}
function addPath(\DOMNode $parent, string $d)
{
    $svg_path = new \DOMElement('path');
    $parent->appendChild($svg_path);
    $svg_path->setAttribute('d', $d);
    $svg_path->setAttribute('fill', '#000000');
    $svg_path->setAttribute('stroke-width', 0.352778);
}
function getSvgContent(float $width, float $height, string $d): string
{
    $doc = getDomDocument();
    $svg = addSvg($doc, $width, $height);
    addPath($svg, $d);
    return $doc->saveXML();
}
function getSvgFilename(string $sourceFilename)
{
    $pinfo = pathinfo($sourceFilename);
    return $pinfo['dirname'] . '/' . $pinfo['filename'] . '.svg';
}
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
    /* The instance of 'Options' class contains default settings now, you can change them using accessor methods.
       See phpinfo() (or php -i) for details.
    */
    $options = new Options();
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
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
} finally {
    $image->clear();
    $image->destroy();
}
