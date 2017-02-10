CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Installation
 * Configuration
 * Troubleshooting
 * Maintainers

INTRODUCTION
------------

Dynamic Links Filter provides a text filter that converts predefined keywords
into links. The text and urls for the links are managed via an administration
page.

 * For a full description of the module, visit the project page:
   https://drupal.org/project/dynamic_links_filter

 * To submit bug reports and feature suggestions, or to track changes:
   https://drupal.org/project/issues/dynamic_links_filter


REQUIREMENTS
------------

No special requirements.


INSTALLATION
------------

Install as you would any other module, see
https://drupal.org/documentation/install/modules-themes/modules-7 for further
information.

* You likely want to install Dynamic Links Filter UI module to manage the
   keyword to url mappings.


CONFIGURATION
-------------

 * Configure user permissions in Administration » People » Permissions:

   - Configure dynamic links filter

     Users in roles with the "configure dynamic links filter" permission are
     able to setup keyword-to-url mappings in Dynamic Links Filter
     configuration section.

 * Add keyword-to-url mappings in Administration » Configuration »
   Content » Dynamic Links Filter.

 * Enable this filter on your desired format(s) in Administration »
   Configuration » Text formats. After enabling the filter, scroll down to
   select the content types you wish to apply this filter to, make any other
   desired changes, then submit.


TROUBLESHOOTING
---------------

 * If keywords are not converting to links, check the following:

   - Did you enable the filter for the content type being viewed?

   - Does the textarea field have the format with the enabled filter?

   - Is the node you're viewing listed in the "Exclude Specific Pages" setting
     of the filter?

   - Is the node displayed in page view (full or default view mode)?

   - Is the keyword misspelled in Dynamic Links Filter configuration?


MAINTAINERS
-----------

Current maintainers:

 * Agnes Chisholm (https://drupal.org/user/amaria)

This project has been sponsored by:
 * GUIDEPOSTS
   Guideposts is a nonprofit organization dedicated to providing hope,
   encouragement, and inspiration to millions of people across America and the
   world.
