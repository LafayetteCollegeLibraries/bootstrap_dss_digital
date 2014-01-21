<?php

/**
 * @file
 * islandora-basic-collection-wrapper.tpl.php
 *
 * @TODO: needs documentation about file and variables
 */
?>

<div class="islandora-basic-collection-wrapper">
  <div class="islandora-basic-collection clearfix">

    <span class="islandora-basic-collection-display-switch">
      <ul class="links inline">
        <?php foreach ($view_links as $label => $link): ?>
          <li>
            <span id="view-<?php print $label ?>-icon" ></span>
            <?php print $link; ?>
          </li>
        <?php endforeach ?>
      </ul>
    </span>

    <?php print $collection_pager; ?>
    <?php print $collection_content; ?>
    <?php print $collection_pager; ?>
  </div>
</div>
