<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html"/>
  <xsl:param name="photoDir">krecipes_recipe_view_photos</xsl:param>
  <xsl:param name="cssStyle">style.css</xsl:param>
  <xsl:param name="imgDir"/>
  <xsl:param name="ingColumns">1</xsl:param>
  <xsl:template match="/">
    <html>
      <head>
        <title>
          <xsl:choose>
            <xsl:when test="count(krecipes-description/title) &gt; 1">Krecipes Recipes</xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="krecipes-description/title"/>
            </xsl:otherwise>
          </xsl:choose>
        </title>
        <link rel="stylesheet" type="text/css" href="{$cssStyle}"/>
      </head>
      <body>
        <xsl:for-each select="krecipes/krecipes-recipe">
          <dl style="background-image: url({$imgDir}/box_bottom.gif)" class="background">
            <dt style="background-image: url({$imgDir}/box_top.gif);" class="title">
              <xsl:value-of select="krecipes-description/title"/>
            </dt>
            <dd>
              <p>
                <xsl:if test="count(krecipes-description/category/cat) &gt; 0">
                  <span class="categories header-data">
                    <span class="header">Categories: </span>
                    <xsl:for-each select="krecipes-description/category/cat">
                      <span class="category">
                        <xsl:value-of select="text()"/>
                      </span>
                      <xsl:if test="last() != position()">, </xsl:if>
                    </xsl:for-each>
                  </span>
                </xsl:if>
                <xsl:if test="count(krecipes-description/author) &gt; 0">
                  <span class="authors header-data">
                    <span class="header">Authors: </span>
                    <xsl:for-each select="krecipes-description/author">
                      <span class="author">
                        <xsl:value-of select="text()"/>
                      </span>
                      <xsl:if test="last() != position()">, </xsl:if>
                    </xsl:for-each>
                  </span>
                  <span class="spacer"> </span>
                </xsl:if>
                <xsl:if test="krecipes-description/yield and krecipes-description/yield/amount &gt; 0">
                  <span class="yield header-data">
                    <span class="header">Yield: </span>
                    <span class="amount">
                      <xsl:value-of select="krecipes-description/yield/amount"/>
                      <xsl:text> </xsl:text>
                    </span>
                    <xsl:if test="krecipes-description/yield/type">
                      <span class="type">
                        <xsl:value-of select="krecipes-description/yield/type"/>
                      </span>
                    </xsl:if>
                  </span>
                  <span class="spacer"> </span>
                </xsl:if>
                <xsl:if test="krecipes-description/preparation-time and krecipes-description/preparation-time/text() != '00:00'">
                  <span class="prep-time header-data">
                    <span class="header">Prep: </span>
                    <xsl:value-of select="krecipes-description/preparation-time"/>
                  </span>
                  <span class="spacer"> </span>
                </xsl:if>
              </p>
              <xsl:if test="count(krecipes-ingredients/*) &gt; 0">
                <p class="ingredients">
                  <h1>Ingredients</h1>
                  <table>
                    <xsl:for-each select="krecipes-ingredients/*">
                      <xsl:choose>
                        <xsl:when test="name() = 'ingredient'">
                          <xsl:call-template name="ingredient"/>
                        </xsl:when>
                        <xsl:otherwise>
                          <tr>
                            <td colspan="4">
                              <div class="ingredient-group">
                                <xsl:value-of select="@name"/>
                              </div>
                            </td>
                          </tr>
                          <xsl:for-each select="ingredient">
                            <xsl:call-template name="ingredient">
                              <xsl:with-param name="underGroup">    </xsl:with-param>
                            </xsl:call-template>
                          </xsl:for-each>
                        </xsl:otherwise>
                      </xsl:choose>
                    </xsl:for-each>
                  </table>
                </p>
              </xsl:if>
              <xsl:if test="krecipes-instructions">
                <p class="instructions">
                  <h1>Instructions</h1>
                  <xsl:call-template name="br-replace">
                    <xsl:with-param name="word" select="krecipes-instructions"/>
                  </xsl:call-template>
                </p>
              </xsl:if>
              <xsl:if test="krecipes-properties">
                <p class="properties">
                  <h1>Properties</h1>
                </p>
              </xsl:if>
              <xsl:if test="count(krecipes-ratings/rating) &gt; 0">
                <p class="ratings">
                  <h1>Ratings</h1>
                  <xsl:for-each select="krecipes-ratings/rating">
                    <span class="rater">
                      <xsl:value-of select="rater"/>
                    </span>
                    <xsl:if test="count(criterion/criteria) &gt; 0">
                      <table>
                        <xsl:for-each select="criterion/criteria">
                          <tr>
                            <td>
                              <span class="criteria-name">
                                <xsl:value-of select="name"/>
                              </span>
                            </td>
                            <td>
                              <xsl:element name="img">
                                <xsl:attribute name="src"><xsl:value-of select="$photoDir"/>/<xsl:value-of select="stars"/>-stars.png</xsl:attribute>
                              </xsl:element>
                            </td>
                          </tr>
                        </xsl:for-each>
                      </table>
                    </xsl:if>
                    <span class="comments">
                      <xsl:value-of select="comment"/>
                    </span>
                    <xsl:if test="position() != last()">
                      <hr/>
                    </xsl:if>
                  </xsl:for-each>
                </p>
              </xsl:if>
            </dd>
          </dl>
          <xsl:if test="position() != last()">
            <br/>
            <br/>
          </xsl:if>
        </xsl:for-each>
      </body>
    </html>
  </xsl:template>
  <xsl:template name="ingredient">
    <xsl:param name="ingSub"/>
    <xsl:param name="underGroup"/>
    <tr>
      <td class="ingredient-amount">
        <xsl:value-of select="$underGroup"/>
        <xsl:value-of select="$ingSub"/>
        <xsl:if test="amount/max != 0 or amount != 0">
          <xsl:choose>
            <xsl:when test="amount/min"><xsl:value-of select="amount/min"/>-<xsl:value-of select="amount/max"/></xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="amount"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:if>
      </td>
      <td class="ingredient-unit">
        <xsl:value-of select="unit"/>
      </td>
      <td class="ingredient-name">
        <xsl:value-of select="name"/>
      </td>
      <td>
        <xsl:for-each select="prep">
          <xsl:value-of select="text()"/>
          <xsl:if test="position() != last()">, </xsl:if>
        </xsl:for-each>
      </td>
    </tr>
    <xsl:if test="count(substitutes/ingredient) &gt; 0">
      <xsl:for-each select="substitutes/ingredient">
        <xsl:call-template name="ingredient">
          <xsl:with-param name="ingSub"> OR </xsl:with-param>
          <xsl:with-param name="underGroup">
            <xsl:value-of select="$underGroup"/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:for-each>
    </xsl:if>
  </xsl:template>
  <xsl:template name="br-replace">
    <xsl:param name="word">
      <xsl:value-of select="."/>
    </xsl:param>
    <xsl:param name="recursing"/>
    <!-- </xsl:text> on next line on purpose to get newline -->
    <xsl:variable name="cr">
      <xsl:text>
</xsl:text>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="contains($word,$cr)">
        <xsl:if test="substring-before($word,$cr) or $recursing">
          <xsl:value-of select="substring-before($word,$cr)"/>
          <br/>
        </xsl:if>
        <xsl:call-template name="br-replace">
          <xsl:with-param name="word" select="substring-after($word,$cr)"/>
          <xsl:with-param name="recursing" select="'1'"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$word"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
