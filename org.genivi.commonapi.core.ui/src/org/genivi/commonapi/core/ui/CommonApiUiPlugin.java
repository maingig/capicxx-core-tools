/* Copyright (C) 2013-2020 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
   This Source Code Form is subject to the terms of the Mozilla Public
   License, v. 2.0. If a copy of the MPL was not distributed with this
   file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package org.genivi.commonapi.core.ui;

import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

public class CommonApiUiPlugin extends AbstractUIPlugin {
	public static final String PLUGIN_ID = "org.genivi.commonapi.core.ui"; //$NON-NLS-1$

	private static CommonApiUiPlugin INSTANCE;

	public CommonApiUiPlugin() {
	}

	@Override
	public void start(final BundleContext context) throws Exception {
		super.start(context);
		INSTANCE = this;
	}

	@Override
	public void stop(final BundleContext context) throws Exception {
		INSTANCE = null;
		super.stop(context);
	}

	public static CommonApiUiPlugin getInstance() {
		return INSTANCE;
	}
	
	public static CommonApiUiPlugin getDefault() {
		return INSTANCE;
	}
	/**
	 * Returns an image descriptor for the image file at the given
	 * plug-in relative path
	 *
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path) {
		return imageDescriptorFromPlugin(PLUGIN_ID, path);
	}
	
	public static IPreferenceStore getValidatorPreferences() {
		return INSTANCE.getPreferenceStore();
	}
	
	
}
