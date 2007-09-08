<?xml version='1.0' encoding='utf-8' ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>

<xsl:param name="photoDir">krecipes_recipe_view_photos</xsl:param>
<xsl:param name="cssStyle">style.css</xsl:param>

<xsl:template match="/">
<html>
<head>
<title>
<xsl:choose>
  <xsl:when test="count(krecipes-description/title) > 1">Krecipes Recipes</xsl:when>
  <xsl:otherwise><xsl:value-of select="krecipes-description/title"/></xsl:otherwise>
</xsl:choose>
</title>
<link rel="stylesheet" type="text/css" href="{$cssStyle}" />
</head>
<body class="background">

<xsl:for-each select="krecipes/krecipes-recipe">
<table width="100%" class="recipes">
  <tr><td colspan="3" width="100%"><div class="title"><xsl:value-of select="krecipes-description/title"/></div></td></tr>
  <tr><td colspan="3"> </td></tr>
  <tr>
    <td width="25%" align="center">
      <xsl:variable name="photoSrc">
        <xsl:choose>
          <xsl:when test="count(krecipes-description/pictures/pic) > 0"><xsl:value-of select="@id"/>.png</xsl:when>
          <xsl:otherwise>default_photo.png</xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <img src="{$photoDir}/{$photoSrc}" class="photo" />
    </td>
    <td width="3%"> </td>
    <td valign="top">
      <xsl:if test="count(krecipes-ratings/rating/criterion/criteria/stars) > 0">
        <p class="overall-rating"><span class="header">Overall Rating: </span> 
        <xsl:element name="img">
          <xsl:attribute name="src"><xsl:value-of
select="$photoDir"/>/<xsl:value-of select="round(2*sum(krecipes-ratings/rating/criterion/criteria/stars) div count(krecipes-ratings/rating/criterion/criteria/stars)) div 2"/>-stars.png</xsl:attribute>
        </xsl:element>
        (<xsl:value-of select="count(krecipes-ratings/rating)"/> reviews)</p>
      </xsl:if>
      <xsl:if test="count(krecipes-description/category/cat) > 0">
      <p class="categories"><span class="header">Categories: </span> 
        <xsl:for-each select="krecipes-description/category/cat">
          <span class="category"><xsl:value-of select="text()"/></span>
          <xsl:if test="last() != position()">, </xsl:if>
        </xsl:for-each>
      </p>
      </xsl:if>
      <xsl:if test="count(krecipes-description/author) > 0">
      <p class="authors"><span class="header">Authors: </span> 
        <xsl:for-each select="krecipes-description/author">
          <span class="author"><xsl:value-of select="text()"/></span>
          <xsl:if test="last() != position()">, </xsl:if>
        </xsl:for-each>
      </p>
      </xsl:if>
      <xsl:if test="krecipes-description/yield and krecipes-description/yield/amount > 0">
      <p class="yield"><span class="header">Yield: </span> 
        <span class="amount"><xsl:value-of select="krecipes-description/yield/amount"/><xsl:text> </xsl:text></span>
        <xsl:if test="krecipes-description/yield/type">
          <span class="type"><xsl:value-of select="krecipes-description/yield/type"/></span>
        </xsl:if>
      </p>
      </xsl:if>
      <xsl:if test="krecipes-description/preparation-time and krecipes-description/preparation-time/text() != '00:00'">
        <p class="prep-time"><span class="header">Prep: </span><xsl:value-of select="krecipes-description/preparation-time"/></p>
      </xsl:if>
    </td>
  </tr>

    <tr><td colspan="3"> </td></tr>

    <tr><td valign="top" class="ingredients">
      <ul>
      <xsl:for-each select="krecipes-ingredients/*">
        <xsl:choose>
        <xsl:when test="name() = 'ingredient'">
          <xsl:call-template name="ingredient"/>
        </xsl:when>
        <xsl:otherwise>
          <li class="ingredient-group"><xsl:value-of select="@name"/></li>
          <ul>
        <xsl:for-each select="ingredient">
          <xsl:call-template name="ingredient"/>
        </xsl:for-each>
        </ul>
        </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>
      </ul>
    </td>
    <td> </td>
    <td valign="top">
      <p class="instructions"><xsl:value-of select="krecipes-instructions"/></p>

  <xsl:if test="count(krecipes-ratings/rating) > 0">
    <div class="ratings"><h1>Ratings</h1>
    <xsl:for-each select="krecipes-ratings/rating">
      <span class="rater"><xsl:value-of select="rater"/></span>
      <xsl:if test="count(criterion/criteria) > 0">
        <table>
        <xsl:for-each select="criterion/criteria">
          <tr>
          <td><span class="criteria-name"><xsl:value-of select="name"/></span></td>
          <td>
          <xsl:element name="img">
            <xsl:attribute name="src"><xsl:value-of
select="$photoDir"/>/<xsl:value-of select="stars"/>-stars.png</xsl:attribute>
          </xsl:element>
          </td>
          </tr>
        </xsl:for-each>
        </table>
      </xsl:if>
      <span class="comments"><xsl:value-of select="comment"/></span>
      
      <xsl:if test="position() != last()"><hr /></xsl:if>
    </xsl:for-each>
  </div>
  </xsl:if>
    </td>
    </tr>
    <tr><td colspan="3"> </td></tr>

  <xsl:if test="krecipes-properties">
    <tr><td valign="top" class="properties"><h1>Properties</h1>
      Properties (TODO)
    </td></tr>
    <tr><td> </td></tr>
  </xsl:if>
</table>
<xsl:if test="position() != last()"><hr size="3" /></xsl:if>
</xsl:for-each>

</body>
</html>
</xsl:template>

<xsl:template name="ingredient">
  <xsl:param name="ingSub"></xsl:param>
        <li>
          <xsl:value-of select="$ingSub"/><span class="ingredient-amount">
          <xsl:if test="amount/min > 0 or amount > 0">
          <xsl:choose>
            <xsl:when test="amount/min">
              <xsl:value-of select="amount/min"/>-<xsl:value-of select="amount/max"/>
            </xsl:when>
            <xsl:otherwise><xsl:value-of select="amount"/></xsl:otherwise>
          </xsl:choose>
          </xsl:if>
          </span><xsl:text> </xsl:text>
          <span class="ingredient-unit"><xsl:value-of select="unit"/></span><xsl:text> </xsl:text>
          <span class="ingredient-name"><xsl:value-of select="name"/></span>
          <span>
            <xsl:for-each select="prep">
              <xsl:if test="position() = 1">; </xsl:if>
              <xsl:value-of select="text()"/>
              <xsl:if test="position() != last()">, </xsl:if>
            </xsl:for-each>
          </span>
        </li>
   <xsl:if test="count(substitutes/ingredient) > 0">
     <ul>
     <xsl:for-each select="substitutes/ingredient">
     <xsl:call-template name="ingredient">
       <xsl:with-param name="ingSub">OR </xsl:with-param>
     </xsl:call-template>
     </xsl:for-each>
     </ul>
   </xsl:if>
</xsl:template>

</xsl:stylesheet>
