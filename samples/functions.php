<?php

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
