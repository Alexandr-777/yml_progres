<?php
/**
 * @file
 * Theme implementation to display single product.
 *
 * Available variables:
 * - $product: custom product object. Contain model, url, price, category,
 *               image, title, description properties
 * - $currency:
 */

 ?>
  <offer id="<?php echo $product->model ?>" available="true">
    <url><?php echo $product->url ?></url>
    <?php /* <vendorCode> echo $product->model; </vendorCode> */?>
    <price><?php echo $product->price ?></price>
    <currencyId><?php echo $currency; ?></currencyId>
    <categoryId><?php echo $product->category ?></categoryId>
    <?php if (!empty($product->image)): 
     foreach ($product->image as $value) {
         echo "<picture>$value</picture>\n";
     }
     endif; ?> 
    <name><?php echo $product->title ?></name>
    <description><?php echo check_plain(strip_tags($product->description)); ?></description>
    <sales_notes></sales_notes>
  </offer>
