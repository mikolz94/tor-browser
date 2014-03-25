/* -*- Mode: Java; c-basic-offset: 4; tab-width: 20; indent-tabs-mode: nil; -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko.home;

import org.mozilla.gecko.R;
import org.mozilla.gecko.db.BrowserContract.HomeItems;
import org.mozilla.gecko.home.HomeConfig.ItemType;

import android.content.Context;
import android.database.Cursor;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.squareup.picasso.Picasso;
import com.squareup.picasso.RequestCreator;

class PanelItemView extends LinearLayout {
    private final TextView title;
    private final TextView description;
    private final ImageView image;
    private final LinearLayout titleDescContainer;
    private int targetWidth = 0;
    private int targetHeight = 0;

    private PanelItemView(Context context, int layoutId) {
        super(context);

        LayoutInflater.from(context).inflate(layoutId, this);
        title = (TextView) findViewById(R.id.title);
        description = (TextView) findViewById(R.id.description);
        image = (ImageView) findViewById(R.id.image);
        titleDescContainer = (LinearLayout) findViewById(R.id.title_desc_container);
    }

    /**
     * Allows setting the size that images are resized to explicitly.
     *
     * @param  width the width that images should be resized to
     * @param  height the height that images should be resized to
     */
    public void setTargetImageSize(int width, int height) {
        targetWidth = width;
        targetHeight = height;
    }

    public void updateFromCursor(Cursor cursor) {
        int titleIndex = cursor.getColumnIndexOrThrow(HomeItems.TITLE);
        final String titleText = cursor.getString(titleIndex);

        // Only show title if the item has one
        final boolean hasTitle = !TextUtils.isEmpty(titleText);
        title.setVisibility(hasTitle ? View.VISIBLE : View.GONE);
        titleDescContainer.setVisibility(hasTitle ? View.VISIBLE : View.GONE);
        if (hasTitle) {
            title.setText(titleText);
        }

        int descriptionIndex = cursor.getColumnIndexOrThrow(HomeItems.DESCRIPTION);
        final String descriptionText = cursor.getString(descriptionIndex);

        // Only show description if the item has one
        final boolean hasDescription = !TextUtils.isEmpty(descriptionText);
        description.setVisibility(hasDescription ? View.VISIBLE : View.GONE);
        if (hasDescription) {
            description.setText(descriptionText);
        }

        titleDescContainer.setVisibility(hasTitle || hasDescription ? View.VISIBLE : View.GONE);

        int imageIndex = cursor.getColumnIndexOrThrow(HomeItems.IMAGE_URL);
        final String imageUrl = cursor.getString(imageIndex);

        // Only try to load the image if the item has define image URL
        final boolean hasImageUrl = !TextUtils.isEmpty(imageUrl);
        image.setVisibility(hasImageUrl ? View.VISIBLE : View.GONE);

        if (hasImageUrl) {
            RequestCreator picasso = Picasso.with(getContext())
                                            .load(imageUrl);

            if (targetWidth != 0 && targetHeight != 0) {
               picasso.resize(targetWidth, targetHeight)
                      .centerCrop();
            }

            picasso.into(image);
        }
    }

    private static class ArticleItemView extends PanelItemView {
        private ArticleItemView(Context context) {
            super(context, R.layout.panel_article_item);
            setOrientation(LinearLayout.HORIZONTAL);
        }
    }

    private static class ImageItemView extends PanelItemView {
        private ImageItemView(Context context) {
            super(context, R.layout.panel_image_item);
            setOrientation(LinearLayout.VERTICAL);
        }
    }

    public static PanelItemView create(Context context, ItemType itemType) {
        switch(itemType) {
            case ARTICLE:
                return new ArticleItemView(context);

            case IMAGE:
                return new ImageItemView(context);

            default:
                throw new IllegalArgumentException("Could not create panel item view from " + itemType);
        }
    }
}
