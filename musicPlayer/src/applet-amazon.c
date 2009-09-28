/**
* This file is a part of the Cairo-Dock project
*
* Copyright : (C) see the 'copyright' file.
* E-mail    : see the 'copyright' file.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _BSD_SOURCE
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "applet-amazon.h"
#include "applet-struct.h"

//#define LICENCE_KEY "0C3430YZ2MVJKQ4JEKG2"
#define LICENCE_KEY "AKIAIAW2QBGIHVG4UIKA"
#define AMAZON_API_URL_1 "http://ecs.amazonaws.com/onca/xml?Service=AWSECommerceService&AWSAccessKeyId="
#define AMAZON_API_URL_2 "&AssociateTag=webservices-20&ResponseGroup=Images,ItemAttributes&Operation=ItemSearch&ItemSearch.Shared.SearchIndex=Music"
/*
 * For the detailed example we'll use a typical ItemLookup request:

http://webservices.amazon.com/onca/xml?Service=AWSECommerceServic
e&AWSAccessKeyId=00000000000000000000&Operation=ItemLookup&ItemId
=0679722769&ResponseGroup=ItemAttributes,Offers,Images,Reviews&Ve
rsion=2009-01-06 

Steps to Sign the Example Request

   1.

      Enter the timestamp. For this example, we'll use GMT time of 2009-01-01T12:00:00Z (%F%T%z)

      http://webservices.amazon.com/onca/xml?Service=AWSECommerceServic
      e&AWSAccessKeyId=00000000000000000000&Operation=ItemLookup&ItemId
      =0679722769&ResponseGroup=ItemAttributes,Offers,Images,Reviews&Ve
      rsion=2009-01-06&Timestamp=2009-01-01T12:00:00Z

   2.

      URL encode the request's comma (,) and colon (;) characters, so that they don't get misinterpreted. For more information about converting to RFC 3986 specifications, see documentation and code samples for your programming language.

      http://webservices.amazon.com/onca/xml?Service=AWSECommerceServic
      e&AWSAccessKeyId=00000000000000000000&Operation=ItemLookup&ItemId
      =0679722769&ResponseGroup=ItemAttributes%2COffers%2CImages%2CRevi
      ews&Version=2009-01-06&Timestamp=2009-01-01T12%3A00%3A00Z

      [Important]	Important

      Be sure that you do not double-escape any characters.
   3.

      Split the parameter/value pairs and delete the ampersand characters (&) so that the example looks like the following:

      Service=AWSECommerceService
      AWSAccessKeyId=00000000000000000000
      Operation=ItemLookup
      ItemId=0679722769
      ResponseGroup=ItemAttributes%2COffers%2CImages%2CReviews
      Version=2009-01-06
      Timestamp=2009-01-01T12%3A00%3A00Z

   4.

      Sort your parameter/value pairs by byte value (not alphabetically, lowercase parameters will be listed after uppercase ones).

      AWSAccessKeyId=00000000000000000000
      ItemId=0679722769
      Operation=ItemLookup
      ResponseGroup=ItemAttributes%2COffers%2CImages%2CReviews
      Service=AWSECommerceService
      Timestamp=2009-01-01T12%3A00%3A00Z
      Version=2009-01-06

   5.

      Rejoin the sorted parameter/value list with ampersands. The result is the canonical string that we'll sign:

      AWSAccessKeyId=00000000000000000000&ItemId=0679722769&Operation=I
      temLookup&ResponseGroup=ItemAttributes%2COffers%2CImages%2CReview
      s&Service=AWSECommerceService&Timestamp=2009-01-01T12%3A00%3A00Z&
      Version=2009-01-06

   6.

      Prepend the following three lines (with line breaks) before the canonical string:

      GET
      webservices.amazon.com
      /onca/xml

   7.

      The string to sign:

      GET
      webservices.amazon.com
      /onca/xml
      AWSAccessKeyId=00000000000000000000&ItemId=0679722769&Operation=I
      temLookup&ResponseGroup=ItemAttributes%2COffers%2CImages%2CReview
      s&Service=AWSECommerceService&Timestamp=2009-01-01T12%3A00%3A00Z&
      Version=2009-01-06

   8.

      Calculate an RFC 2104-compliant HMAC with the SHA256 hash algorithm using the string above with our "dummy" Secret Access Key: 1234567890. For more information about this step, see documentation and code samples for your programming language.

      Nace+U3Az4OhN7tISqgs1vdLBHBEijWcBeCqL5xN9xg=

   9.

      URL encode the plus (+) and equal (=) characters in the signature:

      Nace%2BU3Az4OhN7tISqgs1vdLBHBEijWcBeCqL5xN9xg%3D

  10.

      Add the URL encoded signature to your request and the result is a properly-formatted signed request:

      http://webservices.amazon.com/onca/xml?AWSAccessKeyId=00000000000
      000000000&ItemId=0679722769&Operation=ItemLookup&ResponseGroup=It
      emAttributes%2COffers%2CImages%2CReviews&Service=AWSECommerceServ
      ice&Timestamp=2009-01-01T12%3A00%3A00Z&Version=2009-01-06&Signatu
      re=Nace%2BU3Az4OhN7tISqgs1vdLBHBEijWcBeCqL5xN9xg%3D
*/

//static gchar *TAB_IMAGE_SIZES[2] = {"MediumImage", "LargeImage"};

/**
 * Parse le fichier XML passé en argument
 * à la recherche de l'URL de la pochette
 * @param filename URI du fichier à lire
 */
/*gchar *cd_extract_url_from_xml_file (const gchar *filename)
{
	const xmlChar *name, *value;
	int ret;
	xmlTextReaderPtr reader;
	gboolean flag = FALSE;
	gchar *cResult = NULL;

	reader = xmlReaderForFile(filename, NULL, 0);
	if (reader != NULL)
	{
		ret = xmlTextReaderRead(reader);
		while (ret == 1)  // on parcourt tous les noeuds.
		{
			// nom du noeud.
			name = xmlTextReaderConstName(reader);
			if (name == NULL)
				name = BAD_CAST "--";
			g_print (" node: %s\n", name);
			
			if (strcmp (name, TAB_IMAGE_SIZES[myConfig.iImagesSize]) == 0)
			{
				ret = xmlTextReaderRead(reader);
				name = xmlTextReaderConstName(reader);  // <URL>
				g_print (" final node: %s\n", name);
				
				// valeur associee.
				value = xmlTextReaderConstValue(reader);  // renvoit toujours NULL :-(
				g_print (" => value: %s\n", value);
				
				cResult = g_strdup (value);
				break ;
			}
			
			// on passe au suivant.
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
	}
	else
	{
		cd_warning ("Unable to open %s\n", filename);
	}
	xmlCleanupParser();
	return cResult;
}*/
gchar *cd_extract_url_from_xml_file (const gchar *filename, gchar **artist, gchar **album, gchar **title)
{
	gsize length = 0;
	gchar *cContent = NULL;
	g_file_get_contents (filename,
		&cContent,
		&length,
		NULL);
	g_return_val_if_fail (cContent != NULL, NULL);
	int iWidth, iHeight;
	CD_APPLET_GET_MY_ICON_EXTENT (&iWidth, &iHeight);
	g_print ("cover size : %d\n", iWidth);
	const gchar *cImageSize = (iWidth > 1 && iWidth < 64 ? "SmallImage" : (iWidth < 200 ? "MediumImage" : "LargeImage"));  // small size : 80; medium size : 160; large size : 320
	gchar *str = g_strstr_len (cContent, -1, cImageSize);
	gchar *cResult = NULL;
	if (str)
	{
		str = g_strstr_len (str, -1, "<URL>");
		if (str)
		{
			str += 5;
			gchar *str2 = g_strstr_len (str, -1, "</URL>");
			if (str2)
			{
				cResult = g_strndup (str, str2 - str);
			}
		}
	}
	if (artist != NULL && *artist == NULL)
	{
		str = g_strstr_len (cContent, -1, "<Artist>");
		if (str)
		{
			str += 8;
			gchar *str2 = g_strstr_len (str, -1, "</Artist>");
			if (str2)
			{
				*artist = g_strndup (str, str2 - str);
				g_print ("artist <- %s\n", *artist);
			}
		}
	}
	if (album != NULL && *album == NULL)
	{
		str = g_strstr_len (cContent, -1, "<Album>");
		if (str)
		{
			str += 7;
			gchar *str2 = g_strstr_len (str, -1, "</Album>");
			if (str2)
			{
				*album = g_strndup (str, str2 - str);
				g_print ("album <- %s\n", *album);
			}
		}
	}
	if ((album != NULL && *album == NULL) || (title != NULL && *title == NULL))
	{
		str = g_strstr_len (cContent, -1, "<Title>");
		if (str)
		{
			str += 7;
			gchar *str2 = g_strstr_len (str, -1, "</Title>");
			if (str2)
			{
				gchar *cTitle = g_strndup (str, str2 - str);
				if (album != NULL && *album == NULL)
				{
					str = strchr (cTitle, '/');
					if (str)
					{
						*album = g_strndup (cTitle, str - cTitle);
						g_print ("album <- %s\n", *album);
						if (title != NULL && *title == NULL)
							*title = g_strndup (str+1, str2 - str - 1);
						g_free (cTitle);
						cTitle = NULL;
					}
				}
				if (album != NULL && *album == NULL)
				{
					*album = cTitle;
					g_print ("album <- %s\n", *album);
				}
				else
					g_free (cTitle);
			}
		}
	}
	g_free (cContent);
	return cResult;
}

gchar *cd_get_xml_file (const gchar *artist, const gchar *album, const gchar *cUri)
{
	g_return_val_if_fail ((artist != NULL && album != NULL) || (cUri != NULL), FALSE);
	
	gchar *cFileToDownload;
	
	if (artist != NULL && album != NULL)
	{
		gchar *cKeyWord = g_strdup (album);
		g_strdelimit (cKeyWord, "-_~", ' ');
		gchar *str = cKeyWord;
		for (str = cKeyWord; *str != '\0'; str ++)
		{
			if (*str == ' ')
			{
				*str = '|';
				while (*str == ' ')
					str ++;
			}
			if (*str == '.')
			{
				gchar *ptr;
				for (ptr = str; *ptr != '\0'; ptr ++)
					*ptr = *(ptr+1);
			}
		}
		cFileToDownload = g_strdup_printf ("%s%s%s&Artist=%s&Keywords=%s",
			AMAZON_API_URL_1,
			LICENCE_KEY,
			AMAZON_API_URL_2,
			artist,
			cKeyWord);
		g_free (cKeyWord);
	}
	else
	{
		g_print ("cUri : '%s'\n", cUri);
		gchar *cKeyWord;
		if (*cUri == '/')
		{
			cKeyWord = g_path_get_basename (cUri);
		}
		else
		{
			gchar *cPath = g_filename_from_uri (cUri, NULL, NULL);
			cKeyWord = g_path_get_basename (cPath);
			g_free (cPath);
		}
		g_return_val_if_fail (cKeyWord != NULL, NULL);
		gchar *str = strrchr (cKeyWord, '.');
		if (str)
			*str = '\0';
		g_strdelimit (cKeyWord, "-_~", '|');
		gchar **words = g_strsplit (cKeyWord, "|", -1);
		int i;
		GString *s = g_string_new ("");
		if (words)
		{
			for (i = 0; words[i] != NULL; i ++)
			{
				g_string_append_printf (s, "\"%s\"|", words[i]);
			}
			g_strfreev (words);
		}
		cFileToDownload = g_strdup_printf ("%s%s%s&Keywords=%s",
			AMAZON_API_URL_1,
			LICENCE_KEY,
			AMAZON_API_URL_2,
			s->str);
		g_free (cKeyWord);
		g_string_free (s, TRUE);
	}
	gchar *cTmpFilePath = g_strdup ("/tmp/amazon-cover.XXXXXX");
	int fds = mkstemp (cTmpFilePath);
	if (fds == -1)
	{
		g_free (cTmpFilePath);
		return NULL;
	}
	
	gchar *cCommand = g_strdup_printf ("wget \"%s\" -O \"%s\" -t 3 -T 4 > /dev/null 2>&1", cFileToDownload, cTmpFilePath);
	g_print ("%s\n",cCommand);
	cairo_dock_launch_command (cCommand);
	
	g_free (cCommand);
	g_free (cFileToDownload);
	close(fds);
	return cTmpFilePath;
}

void cd_download_missing_cover (const gchar *cURL)
{
	if (cURL == NULL)
		return ;
	g_return_if_fail (myData.cCoverPath != NULL);
	if (! g_file_test (myData.cCoverPath, G_FILE_TEST_EXISTS))
	{
		gchar *cCommand = g_strdup_printf ("wget \"%s\" -O \"%s\" -t 2 -T 5 > /dev/null 2>&1", cURL, myData.cCoverPath);
		g_print ("%s\n",cCommand);
		cairo_dock_launch_command (cCommand);
		g_free (cCommand);
	}
}
