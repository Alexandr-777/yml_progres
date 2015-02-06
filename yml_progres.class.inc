<?php
/**
 * @file
 * The code processing products and etc in the YML Tools module.
 */

interface YmlProgresStoreTypeInterface {
  /**
   * Get store curency.
   */
  public function getCurrency();

  /**
   * Get price for single product.
   *
   * @param object $product
   *   Product node.
   *
   * @return float
   *   Product price
   */
  public function getProductPrice($product);

  /**
   * Get image for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product image url
   */
  public function getProductImage($product);

  /**
   * Get SKU for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product SKU
   */
  public function getProductSKU($product);
}

/**
 * Abstract class for store type
 */
abstract class YmlProgresStoreType {
  protected $type;
  protected $cache_id;
  protected $exp_category;
  protected $exp_vurobnuk;

  /**
   * Return store type.
   */
  public function getType() {
    return $this->type;
  }
  
  public function getExpCategory() {
    return $this->exp_category;
  }

  public function getCacheId() {
    return $this->cache_id;
  } 
  
  
  public function getExpVurobnuk() {
    return $this->exp_vurobnuk;
  }
  
  /**
   * Get store categories.
   */
  public function getCategories() {
    $categories = taxonomy_get_tree(2);
    return $categories;
  }

  /**
   * Get store products.
   */
  public function getProducts() {
    $products = array();
    $term_field = 'taxonomy_catalog';
    $descr_field = 'body';

    $node_types = array('product');

    $query = new EntityFieldQuery();

    $nodes = array();
    foreach ($node_types as $type) {
      $tmp_nodes = array();
      $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', $type)
        ->propertyCondition('status', 1);
      $result = $query->execute();

      if (!empty($result['node'])) {
        $nids = array_keys($result['node']);
        $tmp_nodes = node_load_multiple($nids);
      }
      $nodes = array_merge($nodes, (array) $tmp_nodes);
    }

    foreach ($nodes as $id => $node) {
      $validate_price = (float) $this->getProductPrice($node);
      /*if (empty($validate_price)) {
        continue;
      }*/

      $term = field_get_items('node', $node, 'taxonomy_catalog');
      if ($id_count = count($term)) {
        $term = $term[$id_count-1];
      }

      $description = field_get_items('node', $node, 'body');
      if (isset($description[0])) {
        $description = $description[0]['value'];
      }
      $vurobn = field_get_items('node', $node, 'field_vurobn');
      if (isset($vurobn[0])) {
         $vurobn = isset($this->exp_vurobnuk[$vurobn[0]['tid']]) ? $this->exp_vurobnuk[$vurobn[0]['tid']] : '';
      } else {
          $vurobn = '';
      }
      $title = check_plain($node->title);
      $url = url('node/' . $node->nid, array('absolute' => TRUE));
      $image = $this->getProductImage($node);
      $price = round((float) $this->getProductPrice($node), 2);
      $model = $this->getProductSKU($node);

      $products[$id] = (object) array(
        'nid' => $node->nid,
        'title' => $title,
        'url' => $url,
        'category' => $term['tid'],
        'portal_category_url' => isset($this->exp_category[$term['tid']]) ? $this->exp_category[$term['tid']]: '',
        'price' => $price,
        'description' => $description,
        'image' => $image,
        'model' => $model,
        'vendor' => $vurobn,
      );
    }
    return $products;
  }

  /**
   * Generate XML.
   */
  public function getXML() {
    $products_objects = $this->getProducts();
    $categories_objects = $this->getCategories();

    // Generate XML for each category and product from templates.
    $categories = '';
    $theme_type = $this->getType();
    foreach ($categories_objects as $category) {
      $category->name = check_plain($category->name);
      // Set the first parent category as the primary parent.
      if (!empty($category->parents[0])) {
        $category->parent = $category->parents[0];
      }
      if (isset($this->exp_category[$category->tid])) {
          $category->portal_url = $this->exp_category[$category->tid]; 
      }
      $categories .= theme($theme_type.'_category', array(
        'category' => $category,
      ));
    }
    $products = '';
    foreach ($products_objects as $product) {
      $products .= theme($theme_type.'_product', array(
        'product' => $product,
        'currency' => $this->getCurrency(),
      ));
    }

    // Return XML.
    //header('Content-type: application/xhtml+xml; charset=utf-8');
    
    $xml = theme($theme_type.'_xml', array(
      'products' => $products,
      'categories' => $categories,
      'currency' => $this->getCurrency(),
    ));
    
    cache_set($this->cache_id, $xml);
    echo $xml;
    exit();
  }

}

/**
 * Ubercart Store Class
 */
class YmlProgresUbercartStore extends YmlProgresStoreType implements YmlProgresStoreTypeInterface {
  /**
   * Implements class constructor.
   */
  public function __construct() {
    $this->type = 'yml_progres_prom';
    $exp_category = db_select('promua_cat', 'n')
        ->fields('n', array('ID', 'URLPromua'))
        ->execute()
        ->fetchAllKeyed();
    $this->exp_category = $exp_category;
    $exp_vurobnuk = db_select('promua_ven', 'n')
        ->fields('n', array('ID', 'VENDOR_PROM'))
        ->execute()
        ->fetchAllKeyed();
    $this->exp_vurobnuk = $exp_vurobnuk;
    $this->cache_id = 'yml_progres_promua_full';
  }

  /**
   * Get content types of product.
   */
  public function getContentTypes() {
    $ctypes = uc_product_types();
    $ctypes = array_combine($ctypes, $ctypes);
    return array_keys($ctypes);
  }

  /**
   * Get store curency.
   */
  public function getCurrency() {
    return variable_get('uc_currency_code', 'USD');
  }

  /**
   * Get price for single product.
   *
   * @param object $product
   *   Product node.
   *
   * @return float
   *   Product price
   */
  public function getProductPrice($product) {
    return $product->sell_price;
  }

  /**
   * Get image for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product image url
   */
  public function getProductImage($product) {
    $image = field_get_items('node', $product, 'uc_product_image');
    $image_paph = array();
    foreach ($image as $value) {
        $image_paph[] = file_create_url($value['uri']);
    }
    $image = field_get_items('node', $product, 'field_galery');
    foreach ($image as $value) {
        $image_paph[] = file_create_url($value['uri']);
    }    
    return $image_paph;
  }

  /**
   * Get SKU for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product SKU
   */
  public function getProductSKU($product) {
    return check_plain($product->model);
  }
}

class YmlProgresYandexUbercartStore extends YmlProgresUbercartStore{
  public function __construct() {
    $this->type = 'yml_progres_yandex';
    $this->exp_category = array();
    $this->exp_vurobnuk = array();
    $this->cache_id = 'yml_progres_yandex';
  }    
}