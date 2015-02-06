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
  <offer id="<?php echo $product->model ?>" available="true" selling_type="r">
    <name><?php echo $product->title ?></name>
    <categoryId><?php echo $product->category ?></categoryId>
    <portal_category_url><?php echo $product->portal_category_url ?></portal_category_url>
    <?php  if ($product->price) :  echo "<price>$product->price</price>\n"; endif; ?>
    <currencyId><?php echo $currency; ?></currencyId>
    <?php if (!empty($product->image)): 
     foreach ($product->image as $value) {
         echo "<picture>$value</picture>\n";
     }
     endif; ?>    
    <?php  if ($product->vendor) :  echo "<vendor>$product->vendor</vendor>\n"; endif; ?>
    <vendorCode><?php  echo $product->model; ?></vendorCode>
    <description><![CDATA[<?php echo $product->description; ?>]]></description>
  </offer>
